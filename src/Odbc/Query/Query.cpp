// File: Query.cpp
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
#include "Query.h"

#include "Odbc/Connection/ConnectionPool.h"



using namespace v8;


#ifdef _DEBUG
extern std::atomic< size_t >	g_nConnectionPoolCount;
#endif


CQuery::CQuery( const std::shared_ptr< CConnectionPool > pPool )
	: CResultSet( this ), m_pPool( pPool )
{
	SetState( EQueryState::eExecuteStatement );
}

CQuery::~CQuery( )
{
	assert( m_pConnection == nullptr );

	CResultSet::Dispose( );

	COdbcStatementHandle::FreeHandle( );
}

EQueryReturn CQuery::Process( )
{
	if( GetState( ) == EQueryState::eExecuteStatement )
	{
		if( !ExecuteStatement( ) )
		{
			SetError( );
			return EQueryReturn::eNeedUv;
		}
	}


	if( GetState( ) == EQueryState::eNeedData )
	{
		return EQueryReturn::eNeedUv;
	}

	if( GetState( ) == EQueryState::eFetchResult )
	{
		if( !CResultSet::FetchResults( ) )
		{
			SetError( );
			return EQueryReturn::eNeedUv;
		}

		SetState( EQueryState::eEnd );
	}

	return EQueryReturn::eNeedUv;
}

void CQuery::ProcessBackground( )
{
	if( GetState( ) == EQueryState::eEnd )
	{
		m_pPool->PushConnection( m_pConnection );
#ifdef _DEBUG
		m_pConnection = nullptr;
#endif
	}

}

EForegroundResult CQuery::ProcessForeground( v8::Isolate* isolate )
{
	if( HasError( ) )
	{
		//-> we can delete UvWorker because we still have a ref to CQuery from v8
		Resolve( isolate, m_pError->ConstructErrorObject( isolate ) );
	}
	else
	{
		Resolve( isolate, ConstructResult( isolate ) );
	}

	return EForegroundResult::eDiscard;
}

bool CQuery::ExecuteStatement( )
{
	if( !AllocHandle( m_pConnection->GetSqlHandle( ) ) )
	{
		return false;
	}

	if( !BindOdbcParameters( ) )
	{
		return false;
	}

	if( m_nQueryTimeout > 0 )
	{
		if( !SetStmtAttr( SQL_ATTR_QUERY_TIMEOUT, ( SQLPOINTER )m_nQueryTimeout, SQL_IS_UINTEGER ) )
		{
			return false;
		}
	}

	SQLRETURN sqlRet = ExecDirect( m_szQuery );

	if( sqlRet == SQL_NEED_DATA )
	{
		SetState( EQueryState::eNeedData );
		return true;
	}
	else if( sqlRet == SQL_NO_DATA )
	{
		CResultSet::m_bExecNoData = true;
	}
	else if( !SQL_SUCCEEDED( sqlRet ) )
	{
		return false;
	}

	SetState( EQueryState::eFetchResult );

	return true;
}

bool CQuery::BindOdbcParameters( )
{
	SQLUSMALLINT nParam = 1;
	if( m_bReturnValue )
	{
		if( !BindParameter( nParam++, SQL_PARAM_OUTPUT, SQL_C_SSHORT, SQL_INTEGER, 0, 0, &m_nReturnValue, 0, &m_nCbReturnValue ) )
		{
			return false;
		}
	}

	for( const auto& i : m_vecParameter )
	{
		if( !BindParameter(
			nParam++,
			SQL_PARAM_INPUT,
			i.m_nValueType,
			i.m_nParameterType,
			i.m_nColumnSize,
			i.m_nDigits,
			i.m_pBuffer,
			i.m_nBufferLen,
			&i.m_strLen_or_IndPtr
		) )
		{
			return false;
		}
	}

	return true;
}

void CQuery::SetError( )
{
	m_pError = GetOdbcError( );
	SetState( EQueryState::eEnd );	
}