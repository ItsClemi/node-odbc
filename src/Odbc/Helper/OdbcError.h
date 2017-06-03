// File: OdbcError.h
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


class COdbcError
{
public:
	static const size_t sStateLength = SQL_SQLSTATE_SIZE + 1;

public:
	COdbcError( );

public:
	void FetchError( SQLSMALLINT fHandleType, SQLHANDLE sqlHandle );

	v8::Local< v8::Value > ConstructErrorObject( v8::Isolate* isolate );

private:
	inline SQLRETURN GetDiagRec( SQLSMALLINT fHandleType, SQLHANDLE sqlHandle, SQLSMALLINT iRecord, SQLWCHAR* szSqlState, SQLINTEGER* pfNativeError, std::wstring* pszErrorMsg )
	{
		SQLSMALLINT nBufferLength = 0;
		SQLSMALLINT nTextLength = 0;

		SQLRETURN sqlRet = SQLGetDiagRecW( fHandleType, sqlHandle, iRecord, szSqlState, pfNativeError, nullptr, nBufferLength, &nTextLength );
		if( !SQL_SUCCEEDED( sqlRet ) )
		{
			return sqlRet;
		}

		nBufferLength = nTextLength + 1;
		pszErrorMsg->resize( nTextLength + 1 );

		return SQLGetDiagRecW( fHandleType, sqlHandle, iRecord, szSqlState, pfNativeError, &pszErrorMsg->at( 0 ), nBufferLength, &nTextLength );
	}

public:
	inline const std::wstring GetMessage( ) const noexcept
	{
		return m_stream.str( );
	}

	inline const wchar_t* GetSqlState( ) const
	{
		return &m_szSqlState[ 0 ];
	}

	inline const SQLINTEGER GetCode( ) const
	{
		return m_nCode;
	}

public:
	inline bool IsSqlState( const wchar_t* szSqlState ) const
	{
		return memcmp( szSqlState, m_szSqlState, SQL_SQLSTATE_SIZEW ) == 0;
	}
	
private:
	wchar_t					m_szSqlState[ sStateLength ];

	SQLINTEGER				m_nCode;
	std::wostringstream		m_stream;

};


