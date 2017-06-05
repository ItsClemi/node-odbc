// File: OdbcHandle.h
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

#include "Odbc/Helper/OdbcError.h"

#ifdef _DEBUG
extern std::atomic< size_t >	g_nSqlHandleCount;
#endif

#ifndef VALIDATE_SQL_RESULT
#define VALIDATE_SQL_RESULT( x )	this->ValidateSqlStatus( L ## #x, x )
#endif

#ifndef VALIDATE_SQL_RESULT_INFO
#define VALIDATE_SQL_RESULT_INFO( status, x ) \
		this->ValidateSqlStatus( L ## #x, x, status )
#endif

#ifndef VALIDATE_SQL_HANDLE
#define VALIDATE_SQL_HANDLE( )		assert( this->GetSqlHandle( ) != SQL_NULL_HANDLE );
#endif





enum class eOdbcHandleType : SQLSMALLINT
{
	eSqlHandleDBC = SQL_HANDLE_DBC,
	eSqlHandleDesc = SQL_HANDLE_DESC,
	eSqlHandleEnv = SQL_HANDLE_ENV,
	eSqlHandleStmt = SQL_HANDLE_STMT,
};

enum class ETransactionType
{
	eSqlCommit = SQL_COMMIT,
	eSqlRollback = SQL_ROLLBACK,
};


template< eOdbcHandleType T >
class TOdbcHandle
{
	static const size_t sSqlStateLength = 8;

public:
	TOdbcHandle( )
	{ }

	virtual ~TOdbcHandle( )
	{
		FreeHandle( );
	}

public:
	inline bool AllocHandle( SQLHANDLE hSqlParent = nullptr )
	{
		assert( m_hSqlHandle == SQL_NULL_HANDLE );

		if( VALIDATE_SQL_RESULT( SQLAllocHandle( ToUnderlyingType( T ), hSqlParent, &m_hSqlHandle ) ) )
		{
#ifdef _DEBUG
			g_nSqlHandleCount++;
#endif
			return true;
		}

		return false;
	}

	inline bool EndTran( ETransactionType eType )
	{
		return VALIDATE_SQL_RESULT( SQLEndTran( ToUnderlyingType( T ), GetSqlHandle( ), ToUnderlyingType( eType ) ) );
	}

public:
	void FreeHandle( )
	{
		if( m_hSqlHandle != SQL_NULL_HANDLE )
		{
			auto sqlRet = SQLFreeHandle( ToUnderlyingType( T ), m_hSqlHandle );
			assert( SQL_SUCCEEDED( sqlRet ) );
			
			m_hSqlHandle = SQL_NULL_HANDLE;

#ifdef _DEBUG
			g_nSqlHandleCount--;
#endif
		}

	}

protected:
	inline bool ValidateSqlStatus( const wchar_t* szFunction, SQLRETURN sqlResult ) const
	{
		if( sqlResult == SQL_SUCCESS )
		{
			return true;
		}

		if( sqlResult == SQL_SUCCESS_WITH_INFO )
		{
			const auto pError = GetOdbcError( );

			__debugbreak( );
			return true;
		}


		assert( GetSqlHandle( ) != SQL_NULL_HANDLE );


		switch( sqlResult )
		{
			case SQL_INVALID_HANDLE:
			{
				break;
			}
			case SQL_NO_DATA:
			{
				break;
			}

			default:
			{
				// 				auto pError = GetOdbcError( );
				// 				__debugbreak( );

				break;
			}
		}

		return false;
	}

	template< size_t _Size >
	inline bool ValidateSqlStatus( const wchar_t* szFunction, SQLRETURN sqlResult, wchar_t( &szStatus )[ _Size ] )
	{
		//static_assert( _Size > SQL_SQLSTATE_SIZE );
		assert( _Size > SQL_SQLSTATE_SIZE );


		if( sqlResult == SQL_SUCCESS )
		{
			szStatus[ 0 ] = '\0';
			return true;
		}

// 		__debugbreak( );
// 
// 		const auto pError = GetOdbcError( );
// 
// 		wcscpy_s( szStatus, pError->GetSqlState( ) );
// 
// 		memcpy_s( &m_szLastSqlStatus, sSqlStateLength * sizeof( wchar_t ), pError->GetSqlState( ), COdbcError::sStateLength * sizeof( wchar_t ) );
// 

		return SQL_SUCCEEDED( sqlResult );
	}

public:
	inline const auto GetOdbcError( ) const
	{
		const auto pError = std::make_shared< COdbcError >( );

		pError->FetchError( ToUnderlyingType< eOdbcHandleType >( T ), GetSqlHandle( ) );

		return pError;
	}


	inline const SQLHANDLE GetSqlHandle( ) const
	{
		return m_hSqlHandle;
	}

	inline const wchar_t* GetLastSqlStatus( )
	{
		return &m_szLastSqlStatus;
	}


private:
	SQLHANDLE	m_hSqlHandle = SQL_NULL_HANDLE;

	wchar_t		m_szLastSqlStatus[ sSqlStateLength ];

};