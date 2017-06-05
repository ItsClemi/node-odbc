// File: OdbcConnectionHandle.h
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

#pragma once

#include "Odbc/Helper/Handle/OdbcHandle.h"

#ifdef _DEBUG
extern std::atomic< size_t >	g_nSqlConnectionCount;
#endif


enum class EConnectionState : size_t
{
	eNone, eConnected, eConnectionLost, eDisconnected, eError
};

class COdbcConnectionHandle : public TOdbcHandle< eOdbcHandleType::eSqlHandleDBC >
{
public:
	COdbcConnectionHandle( )
	{
#ifdef _DEBUG
		g_nSqlConnectionCount++;
#endif

		SetState( EConnectionState::eNone );
	}

	virtual ~COdbcConnectionHandle( )
	{
		if( GetState( ) == EConnectionState::eConnected )
		{
			Disconnect( );
		}

#ifdef _DEBUG
		g_nSqlConnectionCount--;
#endif
	}

public:
	inline bool Disconnect( )
	{
		if( !VALIDATE_SQL_RESULT( SQLDisconnect( GetSqlHandle( ) ) ) )
		{
			assert( false );
			return false;
		}

		SetState( EConnectionState::eDisconnected );
		return true;
	}

	template< size_t _Size >
	inline bool DriverConnect( const std::wstring& szConnectionString, wchar_t( &szStatus )[ _Size ] )
	{
		if( VALIDATE_SQL_RESULT_INFO(
			szStatus,
			SQLDriverConnectW(
				GetSqlHandle( ),
				nullptr,
				const_cast< SQLWCHAR* >( szConnectionString.c_str( ) ),
				static_cast< SQLSMALLINT >( szConnectionString.length( ) ),
				nullptr,
				0,
				nullptr,
				SQL_DRIVER_NOPROMPT
			) ) )
		{
			SetState( EConnectionState::eConnected );
			return true;
		}

		return false;
	}

	inline bool TestDriverConnection( const std::wstring& szConnectionString, std::wstring* szOdbcConnectionString )
	{
		assert( this != nullptr );

		szOdbcConnectionString->resize( 2048 );

		SQLSMALLINT nStringLength = 0;

		if( !SQL_SUCCEEDED(
			SQLDriverConnectW(
				GetSqlHandle( ),
				nullptr,
				const_cast< SQLWCHAR* >( szConnectionString.c_str( ) ),
				static_cast< SQLSMALLINT >( szConnectionString.length( ) ),
				&szOdbcConnectionString->at( 0 ),
				static_cast< SQLSMALLINT >( szOdbcConnectionString->length( ) ),
				&nStringLength,
				SQL_DRIVER_NOPROMPT
			) 
		) )
		{
			return false;
		}

		size_t nLength = szOdbcConnectionString->length( );
		if( nStringLength > nLength && nLength > 4 )
		{
			szOdbcConnectionString->replace( szOdbcConnectionString->end( ) - 4, szOdbcConnectionString->end( ) - 1, 3, L'.' );
		}

		SetState( EConnectionState::eConnected );
		return true;
	}


	inline bool SetConnectionAttribute( SQLINTEGER fAttribute, SQLPOINTER rgbValue, SQLINTEGER cbValue = 0 )
	{
		return VALIDATE_SQL_RESULT( SQLSetConnectAttrW( GetSqlHandle( ), fAttribute, rgbValue, cbValue ) );
	}

	inline bool GetConnectionAttribute( SQLINTEGER fAttribute, SQLPOINTER rgbValue, SQLINTEGER cbValueMax, SQLINTEGER* pcbValue ) const
	{
		return VALIDATE_SQL_RESULT( SQLGetConnectAttrW( GetSqlHandle( ), fAttribute, rgbValue, cbValueMax, pcbValue ) );
	}

	inline bool GetInfo( SQLUSMALLINT fInfoType, SQLPOINTER rgbInfoValue, SQLSMALLINT cbInfoValueMax, SQLSMALLINT* pcbInfoValue ) const
	{
		return VALIDATE_SQL_RESULT( SQLGetInfoW( GetSqlHandle( ), fInfoType, rgbInfoValue, cbInfoValueMax, pcbInfoValue ) );
	}

public:
	inline bool BeginTransaction( )
	{
		return SetConnectionAttribute( SQL_ATTR_AUTOCOMMIT, reinterpret_cast< SQLPOINTER >( SQL_AUTOCOMMIT_OFF ), SQL_IS_UINTEGER );
	}

	inline bool EndTransaction( ETransactionType eType )
	{
		if( !EndTran( eType ) )
		{
			return SetConnectionAttribute( SQL_ATTR_AUTOCOMMIT, reinterpret_cast< SQLPOINTER >( SQL_AUTOCOMMIT_ON ), SQL_IS_UINTEGER );
		}

		return false;
	}

	inline bool GetFunctions( SQLUSMALLINT FunctionId, SQLUSMALLINT* Supported ) const
	{
		return VALIDATE_SQL_RESULT( SQLGetFunctions( GetSqlHandle( ), FunctionId, Supported ) );
	}

public:
	inline const std::wstring GetInfoString( SQLUSMALLINT fInfoType ) const
	{
		SQLSMALLINT nStringLength = 0;

		std::wstring szInfo;
		if( !GetInfo( fInfoType, nullptr, 0, &nStringLength ) )
		{
			return L"";
		}

		if( nStringLength <= 0 )
		{
			return L"";
		}

		szInfo.resize( nStringLength / sizeof( wchar_t ) );

		SQLSMALLINT nNewStringLength = 0;
		if( !GetInfo(
			fInfoType,
			&szInfo.at( 0 ),

			//> add + sizeof( wchar_t ) for nullterminator (not included in size)
			static_cast< SQLSMALLINT >( (szInfo.length( ) + sizeof( wchar_t ) ) * sizeof( wchar_t ) ),
			&nNewStringLength
		) )
		{
			return L"";
		}

		return szInfo;
	}

	inline bool GetInfoNumeric16( SQLUSMALLINT fInfoType, SQLUSMALLINT* pResult ) const
	{
		SQLSMALLINT nIgnored = 0;

		return GetInfo( fInfoType, pResult, sizeof( SQLUSMALLINT ), &nIgnored );
	}

	inline const auto GetDBMSName( ) const
	{
		return GetInfoString( SQL_DBMS_NAME );
	}

	inline const auto GetDBMSVersion( ) const
	{
		return GetInfoString( SQL_DBMS_VER );
	}

	inline const auto GetDriverManagerVersion( ) const
	{
		return GetInfoString( SQL_DM_VER );
	}

	inline const auto GetDriverName( ) const
	{
		return GetInfoString( SQL_DRIVER_NAME );
	}

	inline const auto GetDriverODBCVersion( ) const
	{
		return GetInfoString( SQL_DRIVER_ODBC_VER );
	}

	inline const auto GetDriverVersion( ) const
	{
		return GetInfoString( SQL_DRIVER_VER );
	}

	inline const auto GetDatabaseName( ) const
	{
		return GetInfoString( SQL_DATABASE_NAME );
	}

	inline const auto GetODBCVersion( ) const
	{
		return GetInfoString( SQL_ODBC_VER );
	}

public:
	inline const EConnectionState GetState( ) const
	{
		return m_eState.load( std::memory_order_relaxed );
	}

	inline void SetState( EConnectionState eState )
	{
		m_eState.store( eState, std::memory_order_relaxed );
	}

private:
	std::atomic< EConnectionState >		m_eState;

};