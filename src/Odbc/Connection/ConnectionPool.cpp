// File: ConnectionPool.cpp
// 
// node-odbc (odbc interface for NodeJS)
// 
// Copyright 2017 Clemens Susenbeth
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "stdafx.h"
#include "ConnectionPool.h"

#include "Extension/PoolExtension.h"
#include "Extension/MssqlExtension.h"
#include "Odbc/Query/Query.h"

using namespace v8;


#ifdef _DEBUG
extern std::atomic< size_t >	g_nConnectionPoolCount;
#endif

CConnectionPool::CConnectionPool( )
{
	SetState( EPoolState::eNone );
	m_bDead.store( false, std::memory_order_relaxed );

	memset( &m_props, 0, sizeof( SConnectionProps ) );

#ifdef _DEBUG
	g_nConnectionPoolCount++;

	gEnv->pConnectionTracker->AddPool( this );
#endif
}

CConnectionPool::~CConnectionPool( )
{
#ifdef _DEBUG
	g_nConnectionPoolCount--;

	gEnv->pConnectionTracker->RemovePool( this );
#endif
}

bool CConnectionPool::ReadConnectionProps( v8::Isolate* isolate, v8::Local< v8::Object > value )
{
	HandleScope scope( isolate );
	const auto context = isolate->GetCurrentContext( );

	auto _enableMssqlMars = value->Get( context, Nan::New( "enableMssqlMars" ).ToLocalChecked( ) );
	auto _poolSize = value->Get( context, Nan::New( "poolSize" ).ToLocalChecked( ) );


	if( _enableMssqlMars.IsEmpty( ) || _poolSize.IsEmpty( ) )
	{
		return false;
	}


	{
		auto enableMssqlMars = _enableMssqlMars.ToLocalChecked( );

		if( !enableMssqlMars->IsUndefined( ) )
		{
			if( enableMssqlMars->IsBoolean( ) )
			{
				m_props.bEnableMssqlMars = enableMssqlMars.As< Boolean >( )->BooleanValue( context ).FromJust( );
			}
			else
			{
				Nan::ThrowTypeError( Nan::New( "enableMssqlMars: invalid type" ).ToLocalChecked( ) );
				return false;
			}
		}
	}

	{
		auto poolSize = _poolSize.ToLocalChecked( );

		if( !poolSize->IsUndefined( ) )
		{
			if( poolSize->IsUint32( ) )
			{
				m_props.nPoolSize = poolSize.As< Uint32 >( )->Uint32Value( context ).FromJust( );
			}
			else
			{
				Nan::ThrowTypeError( Nan::New( "poolSize: invalid type" ).ToLocalChecked( ) );
				return false;
			}
		}
	}

	return true;
}

bool CConnectionPool::ReadResilienceStrategy( v8::Isolate* isolate, v8::Local< v8::Object > value )
{
	HandleScope scope( isolate );
	const auto context = isolate->GetCurrentContext( );

	m_vecResilienceErrorCodes.clear( );

	auto _retries = value->Get( context, Nan::New( "retries" ).ToLocalChecked( ) );
	auto _errorCodes = value->Get( context, Nan::New( "errorCodes" ).ToLocalChecked( ) );

	if( _retries.IsEmpty( ) || _errorCodes.IsEmpty( ) )
	{
		return false;
	}

	auto retries = _retries.ToLocalChecked( );
	auto errorCodes = _errorCodes.ToLocalChecked( );

	if( !retries->IsUint32( ) || !errorCodes->IsArray( ) )
	{
		Nan::ThrowTypeError( Nan::New( "invalid type" ).ToLocalChecked( ) );
		return false;
	}

	m_nResilienceRetries = retries.As< Uint32 >( )->Uint32Value( context ).FromJust( );


	auto array = errorCodes.As< Array >( );
	auto nLength = array->Length( );

	for( uint32_t i = 0; i < nLength; i++ )
	{
		auto _errorCode = array->Get( context, i );

		if( _errorCode.IsEmpty( ) )
		{
			return false;
		}

		auto errorCode = _errorCode.ToLocalChecked( );

		if( !errorCode->IsUint32( ) )
		{
			std::ostringstream stream;
			{
				stream << "invalid array element type at " << i;
			}
			Nan::ThrowTypeError( Nan::New( stream.str( ) ).ToLocalChecked( ) );
			return false;
		}

		uint32_t nErrorCode = errorCode.As< Uint32 >( )->Uint32Value( context ).FromJust( );
		m_vecResilienceErrorCodes.push_back( nErrorCode );
	}

	return true;
}

bool CConnectionPool::InitializePool( const std::wstring& szConnectionString, uint32_t nConnectionTimeout )
{
	m_szConnectionString = szConnectionString;
	m_nConnectionTimeout = nConnectionTimeout;

	auto pConnection = CreateConnection( );
	if( pConnection == nullptr )
	{
		Nan::ThrowError( pConnection->GetOdbcError( )->ConstructErrorObject( Isolate::GetCurrent( ) ) );
		return false;
	}

	if( !pConnection->TestDriverConnection( m_szConnectionString, &m_szOdbcConnectionString ) )
	{
		Nan::ThrowError( pConnection->GetOdbcError( )->ConstructErrorObject( Isolate::GetCurrent( ) ) );
		return false;
	}

	m_szDriverName = pConnection->GetDriverName( );
	m_szDriverVersion = pConnection->GetDriverVersion( );
	m_szDatabaseName = pConnection->GetDatabaseName( );
	m_szOdbcVersion = pConnection->GetODBCVersion( );
	m_szDBMSName = pConnection->GetDBMSName( );

	DetectDriverType( );

	if( !TestConnectionFeatures( pConnection ) )
	{
		return false;
	}


	if( m_props.bEnableMssqlMars )
	{
		m_pMarsConnection = pConnection;
	}


	SetState( EPoolState::eReady );
	return true;
}

void CConnectionPool::DetectDriverType( )
{
	if( m_szDBMSName == L"Microsoft SQL Server" )
	{
		m_eDriverType = EDriverType::eMssql;
	}
	else if( m_szDBMSName == L"MySQL" )
	{
		m_eDriverType = EDriverType::eMySql;
	}

	switch( m_eDriverType )
	{
		case EDriverType::eMssql:
		{
			m_pExtension = std::make_unique< CMssqlPoolExtension >( );
			break;
		}
		case EDriverType::eGeneric:
		case EDriverType::eMySql:
		case EDriverType::eMariaDb:
		case EDriverType::eOracle:
		case EDriverType::ePostgreSQL:
		{
			m_pExtension = std::make_unique< CGenericPoolExtension >( );
			break;
		}
	}
}

bool CConnectionPool::TestConnectionFeatures( std::shared_ptr< COdbcConnectionHandle > pConnection )
{
	SQLUSMALLINT fTable[ SQL_API_ODBC3_ALL_FUNCTIONS_SIZE ] = { };

	if( !pConnection->GetFunctions( SQL_API_ODBC3_ALL_FUNCTIONS, &fTable[ 0 ] ) )
	{
		Nan::ThrowError( pConnection->GetOdbcError( )->ConstructErrorObject( Isolate::GetCurrent( ) ) );
		return false;
	}

	const auto vecRequired = CConnectionPool::GetRequiredOdbcApiFunctions( );

	for( const auto& i : vecRequired )
	{
		if( SQL_FUNC_EXISTS( fTable, i ) != SQL_TRUE )
		{
			std::ostringstream stream;
			{
				stream << "your data source odbc driver doesn't support " << i << " operation";
			}
			Nan::ThrowError( Nan::New( stream.str( ) ).ToLocalChecked( ) );

			return false;
		}
	}

	return m_pExtension->TestConnectionFeatures( pConnection );
}


std::shared_ptr< COdbcConnectionHandle > CConnectionPool::CreateConnection( )
{
	auto pConnection = std::make_shared< COdbcConnectionHandle >( );

	if( !pConnection->AllocHandle( gEnv->pOdbcEnv->GetSqlHandle( ) ) )
	{
		return nullptr;
	}

	if( m_nConnectionTimeout > 0 )
	{
		const auto pTimeout = reinterpret_cast< SQLPOINTER >( static_cast< uint64_t >( m_nConnectionTimeout ) );

		if( !pConnection->SetConnectionAttribute( SQL_ATTR_CONNECTION_TIMEOUT, pTimeout ) ||
			!pConnection->SetConnectionAttribute( SQL_ATTR_LOGIN_TIMEOUT, pTimeout )
			)
		{
			return nullptr;
		}
	}

	if( m_props.bEnableMssqlMars )
	{
		if( !pConnection->SetConnectionAttribute( SQL_COPT_SS_MARS_ENABLED, reinterpret_cast< SQLPOINTER >( SQL_MARS_ENABLED_YES ), SQL_IS_UINTEGER ) )
		{
			return nullptr;
		}
	}

	return pConnection;
}

bool CConnectionPool::ConnectToServer( TConnection pConnection )
{
	wchar_t szSqlState[ COdbcError::sStateLength ];

	if( !pConnection->DriverConnect( m_szConnectionString, szSqlState ) )
	{
		return false;
	}

	return true;
}


std::shared_ptr< CQuery > CConnectionPool::CreateQuery( )
{
	assert( !m_pPool.expired( ) );

	return std::make_shared< CQuery >( m_pPool.lock( ) );
}

void CConnectionPool::PushConnection( std::shared_ptr< COdbcConnectionHandle > pConnection )
{
	if( GetState( ) != EPoolState::eReqShutdown )
	{
		std::lock_guard< std::mutex > l( m_cs );
		m_queueConnection.push( pConnection );
	}
}

void CConnectionPool::ExecuteQuery( std::shared_ptr< CQuery > pQuery )
{
	auto pConnection = PopConnection( );

	if( pConnection == nullptr )
	{
		pConnection = CreateConnection( );

		if( pConnection == nullptr )
		{
			assert( 0 );
			return;
		}

		if( !ConnectToServer( pConnection ) )
		{
			assert( 0 );
			return;
		}
	}

	pQuery->SetConnection( pConnection );

	m_nPendingQueries++;

	auto pOperation = new CUvOperation;

	pOperation->RunOperation( pQuery );
}

void CConnectionPool::FinalizeQuery( )
{
	m_nPendingQueries--;

	if( GetState( ) == EPoolState::eReqShutdown )
	{
		if( m_nPendingQueries == 0 )
		{
			ResolveDisconnect( );
		}
	}
}

void CConnectionPool::ResolveDisconnect( )
{
	assert( v8::Isolate::GetCurrent( ) != nullptr );
	auto isolate = v8::Isolate::GetCurrent( );

	const auto fnDisconnect = node::PersistentToLocal( isolate, m_fnDisconnect );

	Nan::TryCatch try_catch;

	node::MakeCallback( isolate, Object::New( isolate ), fnDisconnect, 0, nullptr );

	if( try_catch.HasCaught( ) )
	{
		try_catch.ReThrow( );
	}

	if( !m_fnDisconnect.IsEmpty( ) )
	{
		m_fnDisconnect.Reset( );
	}
}

Local< Value > CConnectionPool::GetConnectionInfo( Isolate* isolate )
{
	EscapableHandleScope scope( isolate );
	const auto context = isolate->GetCurrentContext( );

	auto obj = Object::New( isolate );
	{
		auto nLength = static_cast< uint32_t >( m_vecResilienceErrorCodes.size( ) );
		auto errorCodes = Array::New( isolate, nLength );

		for( uint32_t i = 0; i < nLength; i++ )
		{
			errorCodes->Set( i, Uint32::New( isolate, m_vecResilienceErrorCodes[ i ] ) );
		}

		auto resilienceStrategy = Object::New( isolate );
		if( resilienceStrategy->Set( context, Nan::New( "retries" ).ToLocalChecked( ), Uint32::New( isolate, m_nResilienceRetries ) ).IsNothing( ) ||
			resilienceStrategy->Set( context, Nan::New( "errorCodes" ).ToLocalChecked( ), errorCodes ).IsNothing( )
			)
		{
			Nan::ThrowError( Nan::New( "failed to create resilience object" ).ToLocalChecked( ) );
			return scope.Escape(
				Undefined( isolate )
			);
		}

		if( obj->Set( context, Nan::New( "driverName" ).ToLocalChecked( ), ToV8String( m_szDriverName ) ).IsNothing( ) ||
			obj->Set( context, Nan::New( "driverVersion" ).ToLocalChecked( ), ToV8String( m_szDriverVersion ) ).IsNothing( ) ||
			obj->Set( context, Nan::New( "databaseName" ).ToLocalChecked( ), ToV8String( m_szDatabaseName ) ).IsNothing( ) ||
			obj->Set( context, Nan::New( "odbcVersion" ).ToLocalChecked( ), ToV8String( m_szOdbcVersion ) ).IsNothing( ) ||
			obj->Set( context, Nan::New( "dbmsName" ).ToLocalChecked( ), ToV8String( m_szDBMSName ) ).IsNothing( ) ||
			obj->Set( context, Nan::New( "internalServerType" ).ToLocalChecked( ), Nan::New( static_cast< double >( ToUnderlyingType( m_eDriverType ) ) ) ).IsNothing( ) ||
			obj->Set( context, Nan::New( "odbcConnectionString" ).ToLocalChecked( ), ToV8String( m_szOdbcConnectionString ) ).IsNothing( ) ||
			obj->Set( context, Nan::New( "resilienceStrategy" ).ToLocalChecked( ), resilienceStrategy ).IsNothing( )
			)
		{
			Nan::ThrowError( Nan::New( "failed to create info object" ).ToLocalChecked( ) );
			return scope.Escape(
				Undefined( isolate )
			);
		}
	}
	return scope.Escape( obj );
}