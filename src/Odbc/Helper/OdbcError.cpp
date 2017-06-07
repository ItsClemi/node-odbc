// File: OdbcError.cpp
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
#include "OdbcError.h"

using namespace v8;


COdbcError::COdbcError( )
{
	m_szSqlState[ 0 ] = L'\0';
}

void COdbcError::FetchError( SQLSMALLINT fHandleType, SQLHANDLE sqlHandle )
{
	SQLSMALLINT it = 0;

	std::wstring szMessage;
	SQLRETURN sqlRet = GetDiagRec( fHandleType, sqlHandle, ++it, &m_szSqlState[ 0 ], &m_nCode, &szMessage );

	if( sqlRet != SQL_NO_DATA )
	{
		m_stream << L"[" << m_szSqlState << L"](" << m_nCode << L") " << szMessage;
	}
	else
	{
		m_stream << L"no error data (SQL_NO_DATA)";
		return;
	}

	assert( SQL_SUCCEEDED( sqlRet ) );
	
	while( sqlRet != SQL_NO_DATA )
	{
		SQLINTEGER nError = 0;
		SQLWCHAR szSqlState[ SQL_SQLSTATE_SIZE + 1 ] = { };

		sqlRet = GetDiagRec( fHandleType, sqlHandle, ++it, szSqlState, &nError, &szMessage );
		{
			m_stream << m_stream.widen( '\n' ) << L"(" << it << L")"  << L"[" << m_szSqlState << L"](" << m_nCode << L")" << szMessage;
		}
	}
}

Local< Value > COdbcError::ConstructErrorObject( Isolate* isolate )
{
	EscapableHandleScope scope( isolate );
	const auto context = isolate->GetCurrentContext( );

	const auto err = Object::New( isolate );
	{
		auto szSqlState = String::NewFromTwoByte( isolate, reinterpret_cast< const uint16_t* >( GetSqlState( ) ), NewStringType::kNormal, SQL_SQLSTATE_SIZE ).ToLocalChecked( );

		if( err->Set( context, Nan::New( "message" ).ToLocalChecked( ), ToV8String( GetMessage( ) ) ).IsNothing( ) ||
			err->Set( context, Nan::New( "sqlState" ).ToLocalChecked( ), szSqlState ).IsNothing( ) ||
			err->Set( context, Nan::New( "code" ).ToLocalChecked( ), Integer::New( isolate, GetCode( ) ) ).IsNothing( )
			)
		{
			assert( false );

			return scope.Escape(
				v8::Null( isolate )
			);
		}
	}

	return scope.Escape( err );
}
