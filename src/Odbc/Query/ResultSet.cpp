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

}

void CResultSet::Dispose( )
{
	assert( Isolate::GetCurrent( ) != nullptr );

	if( m_eResolveType == EResolveType::eCallback )
	{
		m_callback.Reset( );
	}
	else if( m_eResolveType == EResolveType::ePromise )
	{
		m_resolve.Reset( );
		m_reject.Reset( );
	}
}

bool CResultSet::FetchResults( )
{
	if( GetState( ) == EResultState::eNone )
	{
		//SetState()
	}

	return true;
}

void CResultSet::ProcessBackground( )
{

}

EForegroundResult CResultSet::ProcessForeground( v8::Isolate* isolate )
{
	return EForegroundResult::eDiscard;
}

bool CResultSet::DrainRemainingResults( )
{
	for( ;; )
	{
		SQLRETURN sqlRet = m_pQuery->MoreResults( );

		if( sqlRet == SQL_NO_DATA )
		{
			break;
		}
		else if( !SQL_SUCCEEDED( sqlRet ) )
		{
			return false;
		}
	}

	return true;
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
}

/*

switch( pDefinition->m_nDataType )
{
	case SQL_VARCHAR:
	case SQL_LONGVARCHAR:
	case SQL_WCHAR:
	case SQL_WVARCHAR:
	case SQL_WLONGVARCHAR:
	case SQL_VARBINARY:
	case SQL_LONGVARBINARY:
	{
		if( pDefinition->m_nColumnSize == 0 )
		{
			pDefinition->m_bLOBColumn = true;
			m_bHasLOBTypes = true;
}

*/