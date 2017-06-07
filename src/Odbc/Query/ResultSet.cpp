// File: ResultSet.cpp
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
#include "ResultSet.h"

#include "Odbc/Query/Query.h"


using namespace v8;


CResultSet::CResultSet( CQuery* pQuery )
	: m_pQuery( pQuery )
{
	SetState( EResultState::eNone );
}

CResultSet::~CResultSet( )
{
	assert( Isolate::GetCurrent( ) != nullptr );

	for( auto& i : m_vecData )
	{
		i.Dispose( );
	}

	m_vecMetaData.clear( );

	if( m_eResolveType == EResolveType::eCallback )
	{
		m_callback.Reset( );
	}
	else if( m_eResolveType == EResolveType::ePromise )
	{
		m_resolve.Reset( );
		m_reject.Reset( );
	}

	if( !m_queryInstance.IsEmpty( ) )
	{
		m_queryInstance.Reset( );
	}
}

bool CResultSet::FetchResults( )
{
	if( !FindNextResultSet( ) )
	{
		return false;
	}

	if( m_nColumns <= 0 )
	{
		return true;
	}

	if( !PrepareColumns( ) )
	{
		return false;
	}

	size_t nChunkSize = 1;
	if( m_eFetchMode == EFetchMode::eArray )
	{
		nChunkSize = sDefaultChunkSize;
	}

	m_vecData.resize( m_nColumns * nChunkSize );

	size_t it = 0;
	for( ;; )
	{
		auto eResult = FetchChunk( nChunkSize, &it );

		if( eResult == EFecthResult::eError )
		{
			return false;
		}
		else if( eResult == EFecthResult::eHasMoreData )
		{
			if( m_eFetchMode == EFetchMode::eSingle )
			{
				break;
			}

			if( ( it ) * m_nColumns >= m_vecData.size( ) )
			{
				m_vecData.resize( m_vecData.size( ) * 2 );
			}
		}
		else if( eResult == EFecthResult::eDone )
		{
			break;
		}
	}

	m_vecData.resize( it * static_cast< size_t >( m_nColumns ) );

	return true;
}


bool CResultSet::PrepareColumns( )
{
	m_vecMetaData.resize( m_nColumns );

	for( size_t i = 0; i < m_nColumns; i++ )
	{
		auto pMetaData = GetMetaData( i );

		pMetaData->m_bLOBColumn = false;

		if( !m_pQuery->GetStatement( )->DescribeCol(
			static_cast< SQLUSMALLINT >( i + 1 ),
			&pMetaData->m_szColumnName,
			&pMetaData->m_nDataType,
			&pMetaData->m_nColumnSize,
			&pMetaData->m_nDecimalDigits,
			&pMetaData->m_bNullable
		) )
		{
			return false;
		}

		if( m_pQuery->GetResultSet( )->IsMetaDataEnabled( ) )
		{
			if( !m_pQuery->GetStatement( )->ColAttribute(
				static_cast< SQLUSMALLINT >( i + 1 ),
				SQL_DESC_TYPE_NAME,
				&pMetaData->m_szDataTypeName,
				nullptr
			) )
			{
				return false;
			}
		}

		switch( pMetaData->m_nDataType )
		{
			case SQL_LONGVARCHAR:
			case SQL_WLONGVARCHAR:
			case SQL_LONGVARBINARY:
			{
				if( pMetaData->m_nColumnSize == 0 )
				{
					pMetaData->m_bLOBColumn = true;
					m_bHasLobColumns = true;
				}
			}
		}
	}

	return true;
}

bool CResultSet::FindNextResultSet( )
{
	for( ;; )
	{
		//> skip first result sets (empty)
		if( !m_bExecNoData )
		{
			SQLSMALLINT nColumns = 0;
			if( !m_pQuery->GetStatement( )->NumResultCols( &nColumns ) )
			{
				return false;
			}

			m_nColumns = static_cast< size_t >( nColumns );
			return true;
		}
		else
		{
			m_bExecNoData = false;
		}


		auto sqlRet = m_pQuery->GetStatement( )->MoreResults( );

		if( sqlRet == SQL_NO_DATA )
		{
			SetState( EResultState::eDone );
			return true;
		}
		else if( !SQL_SUCCEEDED( sqlRet ) )
		{
			return false;
		}
	}

	return true;
}

EFecthResult CResultSet::FetchChunk( size_t nChunkSize, size_t* nFetched )
{
	for( size_t it = 0; it < nChunkSize; it++ )
	{
		auto sqlRet = m_pQuery->GetStatement( )->FetchScroll( SQL_FETCH_NEXT, 1 );
		if( sqlRet == SQL_NO_DATA )
		{
			return EFecthResult::eDone;
		}
		else if( !SQL_SUCCEEDED( sqlRet ) )
		{
			return EFecthResult::eError;
		}

		for( size_t i = 0; i < static_cast< size_t >( m_nColumns ); i++ )
		{
			if( !ReadColumn( i ) )
			{
				return EFecthResult::eError;
			}
		}

		( *nFetched )++;
		m_nActiveRow++;
	}

	return EFecthResult::eHasMoreData;
}


bool CResultSet::ReadColumn( size_t nColumn )
{
	auto pData = GetColumnData( nColumn );
	auto pMetaData = GetMetaData( nColumn );

	SQLLEN strLen_or_IndPtr;
	switch( pMetaData->m_nDataType )
	{
		case SQL_CHAR:
		case SQL_VARCHAR:
		case SQL_GUID:
		{
			const size_t nBufferSize = pMetaData->m_nColumnSize + sizeof( char );

			auto pBuffer = new char[ nBufferSize ];//static_cast< char* >( scalable_malloc( nBufferSize ) );

			if( !GetSqlData( nColumn, SQL_C_CHAR, pBuffer, nBufferSize, &strLen_or_IndPtr ) )
			{
				return false;
			}

			if( strLen_or_IndPtr == SQL_NULL_DATA )
			{
				pData->SetNull( );
				delete[ ] pBuffer;
				//scalable_free( pBuffer );
			}
			else
			{
				pData->SetString( pBuffer, static_cast< size_t >( strLen_or_IndPtr ) );
			}

			break;
		}

		case SQL_WCHAR:
		case SQL_WVARCHAR:
		{
			const size_t nBufferSize = pMetaData->m_nColumnSize + sizeof( char );
			auto pBuffer = new wchar_t[ nBufferSize ]; //static_cast< wchar_t* >( scalable_malloc( nBufferSize * sizeof( wchar_t ) ) );

			if( !GetSqlData( nColumn, SQL_C_WCHAR, pBuffer, nBufferSize * sizeof( wchar_t ), &strLen_or_IndPtr ) )
			{
				return false;
			}

			if( strLen_or_IndPtr == SQL_NULL_DATA )
			{
				pData->SetNull( );
				delete[ ] pBuffer;
				//scalable_free( pBuffer );
			}
			else
			{
				pData->SetString( pBuffer, static_cast< size_t >( strLen_or_IndPtr ) );
			}

			break;
		}
		case SQL_BIT:
		{
			bool bResult;
			if( !GetSqlData( nColumn, SQL_C_BIT, &bResult, sizeof( bool ), &strLen_or_IndPtr ) )
			{
				return false;
			}

			if( strLen_or_IndPtr == SQL_NULL_DATA )
			{
				pData->SetNull( );
			}
			else
			{
				pData->SetBool( bResult );
			}

			break;
		}
		case SQL_TINYINT:
		case SQL_SMALLINT:
		case SQL_INTEGER:
		{
			int32_t nResult;
			if( !GetSqlData( nColumn, SQL_C_SLONG, &nResult, sizeof( int32_t ), &strLen_or_IndPtr ) )
			{
				return false;
			}

			if( strLen_or_IndPtr == SQL_NULL_DATA )
			{
				pData->SetNull( );
			}
			else
			{
				pData->SetInt32( nResult );
			}

			break;
		}

		case SQL_BIGINT:
		{
			int64_t nResult;
			if( !GetSqlData( nColumn, SQL_C_SBIGINT, &nResult, sizeof( int64_t ), &strLen_or_IndPtr ) )
			{
				return false;
			}

			if( strLen_or_IndPtr == SQL_NULL_DATA )
			{
				pData->SetNull( );
			}
			else
			{
				pData->SetInt64( nResult );
			}

			break;
		}
		case SQL_REAL:
		case SQL_FLOAT:
		case SQL_DOUBLE:
		{
			double dResult;
			if( !GetSqlData( nColumn, SQL_C_DOUBLE, &dResult, sizeof( double ), &strLen_or_IndPtr ) )
			{
				return false;
			}

			if( strLen_or_IndPtr == SQL_NULL_DATA )
			{
				pData->SetNull( );
			}
			else
			{
				pData->SetDouble( dResult );
			}

			break;
		}
		case SQL_DECIMAL:
		case SQL_NUMERIC:
		{
			SQL_NUMERIC_STRUCT numeric;
			if( !GetSqlData( nColumn, SQL_C_NUMERIC, &numeric, sizeof( SQL_NUMERIC_STRUCT ), &strLen_or_IndPtr ) )
			{
				return false;
			}

			if( strLen_or_IndPtr == SQL_NULL_DATA )
			{
				pData->SetNull( );
			}
			else
			{
				pData->SetNumeric( numeric );
			}

			break;
		}
		case SQL_TYPE_TIMESTAMP:
		case SQL_TYPE_TIME:
		case SQL_DATETIME:
		{
			SQL_TIMESTAMP_STRUCT timestamp;
			if( !GetSqlData( nColumn, SQL_C_TIMESTAMP, &timestamp, sizeof( SQL_TIMESTAMP_STRUCT ), &strLen_or_IndPtr ) )
			{
				return false;
			}

			if( strLen_or_IndPtr == SQL_NULL_DATA )
			{
				pData->SetNull( );
			}
			else
			{
				pData->SetTimestamp( timestamp );
			}

			break;
		}
		case SQL_TYPE_DATE:
		{
			SQL_DATE_STRUCT date;
			if( !GetSqlData( nColumn, SQL_C_DATE, &date, sizeof( SQL_DATE_STRUCT ), &strLen_or_IndPtr ) )
			{
				return false;
			}

			if( strLen_or_IndPtr == SQL_NULL_DATA )
			{
				pData->SetNull( );
			}
			else
			{
				pData->SetDate( date );
			}

			break;
		}
		case SQL_BINARY:
		case SQL_VARBINARY:
		{
			const size_t nBufferSize = pMetaData->m_nColumnSize + 1;

			auto pBuffer = new uint8_t[ nBufferSize ];
			memset( pBuffer, 0, nBufferSize );
			//auto pBuffer = static_cast< uint8_t* >( scalable_malloc( nBufferSize ) );

			if( !GetSqlData( nColumn, SQL_C_BINARY, pBuffer, nBufferSize, &strLen_or_IndPtr ) )
			{
				SafeDeleteArray( pBuffer );
				return false;
			}

			if( strLen_or_IndPtr == SQL_NULL_DATA )
			{
				SafeDeleteArray( pBuffer );
				pData->SetNull( );
			}
			else
			{
				pData->SetBuffer( pBuffer, strLen_or_IndPtr );
			}

			break;
		}
		//> LOB types
		case SQL_LONGVARBINARY:
		case SQL_WLONGVARCHAR:
		case SQL_LONGVARCHAR:
		{
			return false;
			break;
		}


		default:
		{
			return false;
		}
	}

	return true;
}

bool CResultSet::GetSqlData( size_t ColumnNumber, SQLSMALLINT TargetType, SQLPOINTER TargetValue, SQLLEN BufferLength, SQLLEN *StrLen_or_IndPtr )
{
	auto sqlRet = m_pQuery->GetStatement( )->GetData( static_cast< SQLUSMALLINT >( ColumnNumber + 1 ), TargetType, TargetValue, BufferLength, StrLen_or_IndPtr );

	if( sqlRet == SQL_SUCCESS_WITH_INFO )
	{
		auto pError = m_pQuery->GetStatement( )->GetOdbcError( );

		if( pError->IsSqlState( L"01004" ) )
		{
			assert( false );
		}
	}

	return SQL_SUCCEEDED( sqlRet );
}

void CResultSet::Resolve( v8::Isolate* isolate, v8::Local< v8::Value > value )
{
	HandleScope scope( isolate );

	if( m_eResolveType == EResolveType::eCallback )
	{
		const unsigned argc = 2;
		Local< Value > argv[ argc ];

		if( !m_pQuery->HasError( ) )
		{
			argv[ 0 ] = value;
			argv[ 1 ] = Null( isolate );
		}
		else
		{
			argv[ 0 ] = Null( isolate );
			argv[ 1 ] = value;
		}

		const auto fnCallback = node::PersistentToLocal( isolate, m_callback );

		Nan::TryCatch try_catch;

		node::MakeCallback( isolate, Object::New( isolate ), fnCallback, argc, argv );

		if( try_catch.HasCaught( ) )
		{
			try_catch.ReThrow( );
		}
	}
	else if( m_eResolveType == EResolveType::ePromise )
	{
		Local< Function > fnPromise;
		if( !m_pQuery->HasError( ) )
		{
			fnPromise = node::PersistentToLocal( isolate, m_resolve );
		}
		else
		{
			fnPromise = node::PersistentToLocal( isolate, m_reject );
		}

		const unsigned argc = 1;
		Local< Value > argv[ argc ] = { value };

		Nan::TryCatch try_catch;

		node::MakeCallback( isolate, Object::New( isolate ), fnPromise, argc, argv );

		if( try_catch.HasCaught( ) )
		{
			try_catch.ReThrow( );
		}
	}
	else
	{
		assert( m_result.IsEmpty( ) );

		//-> store pending result
		m_result.Reset( isolate, value );
	}
}

Local< Value > CResultSet::ConstructResult( Isolate* isolate )
{
	EscapableHandleScope scope( isolate );
	const auto context = isolate->GetCurrentContext( );

	Local< Value > value;
	if( m_eFetchMode == EFetchMode::eSingle )
	{
		value = ConstructResultRow( isolate, 0 );
	}
	else if( m_eFetchMode == EFetchMode::eArray )
	{
		const auto nRows = static_cast< uint32_t >( m_vecData.size( ) / m_nColumns );

		auto array = Array::New( isolate, static_cast< int >( nRows ) );

		for( uint32_t i = 0; i < static_cast< uint32_t >( nRows ); i++ )
		{
			if( array->Set( context, i, ConstructResultRow( isolate, i ) ).IsNothing( ) )
			{
				return scope.Escape( Null( isolate ) );
			}
		}

		value = array;
	}
	else
	{
		assert( false );
	}

	AddResultExtensions( isolate, value.As< Object >( ) );

	return scope.Escape( value );
}

Local< Value > CResultSet::ConstructResultRow( Isolate* isolate, int nRow )
{
	EscapableHandleScope scope( isolate );
	const auto context = isolate->GetCurrentContext( );

	auto value = Object::New( isolate );

	for( size_t i = 0; i < m_nColumns; i++ )
	{
		if( value->Set( context, ToV8String( isolate, GetMetaData( i )->m_szColumnName ), GetColumnData( nRow, i )->ToValue( isolate ) ).IsNothing( ) )
		{
			return Null( isolate );
		}
	}

	return scope.Escape( value );
}

void CResultSet::AddResultExtensions( v8::Isolate* isolate, v8::Local< v8::Object > value )
{
	HandleScope scope( isolate );

	if( IsMetaDataEnabled( ) )
	{
		AddMetaDataExtension( isolate, value );
	}

	if( m_pQuery->IsReturnValueEnabled( ) )
	{
		AddReturnValueExtension( isolate, value );
	}

	if( !m_queryInstance.IsEmpty( ) )
	{
		AddQueryInstanceExtension( isolate, value );
	}
}


void CResultSet::AddMetaDataExtension( v8::Isolate* isolate, v8::Local< v8::Object > value )
{
	HandleScope scope( isolate );
	const auto context = isolate->GetCurrentContext( );

	const auto key = Nan::New( "$sqlMetaData" ).ToLocalChecked( );

	auto _contains = value->Has( context, key );
	if( _contains.IsNothing( ) || _contains.IsJust( ) )
	{
		//> dont override results
		return;
	}


	auto array = Array::New( isolate, static_cast< int >( m_nColumns ) );

	for( size_t i = 0; i < m_nColumns; i++ )
	{
		const auto pMetaData = GetMetaData( i );

		auto entry = Object::New( isolate );
		{
			if( !entry->Set( context, Nan::New( "name" ).ToLocalChecked( ), ToV8String( isolate, pMetaData->m_szColumnName ) ).IsNothing( ) ||
				!entry->Set( context, Nan::New( "size" ).ToLocalChecked( ), Uint32::New( isolate, static_cast< uint32_t >( pMetaData->m_nColumnSize ) ) ).IsNothing( ) ||
				!entry->Set( context, Nan::New( "dataType" ).ToLocalChecked( ), ToV8String( isolate, pMetaData->m_szDataTypeName ) ).IsNothing( ) ||
				!entry->Set( context, Nan::New( "digits" ).ToLocalChecked( ), Uint32::New( isolate, pMetaData->m_nDecimalDigits ) ).IsNothing( ) ||
				!entry->Set( context, Nan::New( "nullable" ).ToLocalChecked( ), Boolean::New( isolate, pMetaData->m_bNullable ) ).IsNothing( )
				)
			{
				return;
			}
		}

		auto _res = array->Set( context, static_cast< uint32_t >( i ), entry );
		if( _res.IsNothing( ) || !_res.FromJust( ) )
		{
			return;
		}
	}

	if( value->Set( context, key, array ).IsNothing( ) )
	{
		return;
	}
}

void CResultSet::AddReturnValueExtension( v8::Isolate* isolate, v8::Local< v8::Object > value )
{
	HandleScope scope( isolate );
	const auto context = isolate->GetCurrentContext( );

	const auto key = Nan::New( "$sqlReturnValue" ).ToLocalChecked( );

	auto _contains = value->Has( context, key );
	if( _contains.IsNothing( ) || _contains.IsJust( ) )
	{
		return;
	}

	if( value->Set( context, key, Int32::New( isolate, m_pQuery->GetReturnValue( ) ) ).IsNothing( ) )
	{
		return;
	}
}

void CResultSet::AddQueryInstanceExtension( v8::Isolate* isolate, v8::Local< v8::Object > value )
{
	HandleScope scope( isolate );
	const auto context = isolate->GetCurrentContext( );

	const auto key = Nan::New( "$sqlQuery" ).ToLocalChecked( );

	auto _contains = value->Has( context, key );
	if( _contains.IsNothing( ) || _contains.IsJust( ) )
	{
		return;
	}

	auto instance = node::PersistentToLocal( isolate, m_queryInstance );

	if( value->Set( context, key, instance ).IsNothing( ) )
	{
		return;
	}
}

void CResultSet::SetPromise( v8::Isolate* isolate, v8::Local< v8::Function > fnResolver, v8::Local< v8::Function > fnRejector )
{
	HandleScope scope( isolate );

	assert( v8::Isolate::GetCurrent( ) != nullptr );
	assert( m_eResolveType == EResolveType::eNone );

	m_eResolveType = EResolveType::ePromise;

	m_resolve.Reset( isolate, fnResolver );
	m_reject.Reset( isolate, fnRejector );


	if( m_pQuery->GetState( ) == EQueryState::eEnd && !m_result.IsEmpty( ) )
	{
		auto result = node::PersistentToLocal( isolate, m_result );

		Resolve( isolate, result );
	}
}

void CResultSet::SetError( )
{
	SetState( EResultState::eDone );
}