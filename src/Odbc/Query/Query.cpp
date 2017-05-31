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
	m_pPool->PushConnection( m_pConnection );

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
	}

	return EQueryReturn::eNeedUv;
}

void CQuery::ProcessBackground( )
{

}

EForegroundResult CQuery::ProcessForeground( v8::Isolate* isolate )
{
	if( HasError( ) )
	{
		Resolve( isolate, m_pError->ConstructErrorObject( isolate ) );
	}
	else
	{

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
		CResultSet::m_bExecIsNullData = true;
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
	if( m_bHasReturnValue )
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
}

//bool CResultSet::ReadColumn( SQLUSMALLINT nColumn )
//{
//	/*	MetaDefinition* pMeta = &m_pMetaData[ nColumn ];
//	sColumnData* pColumn = &m_pActiveRow[ nColumn ];
//
//	SQLLEN strLen_or_IndPtr;
//
//	switch( pMeta->m_nDataType )
//	{
//	case SQL_CHAR:
//	case SQL_VARCHAR:
//	case SQL_LONGVARCHAR:
//	case SQL_WCHAR:
//	case SQL_WVARCHAR:
//	case SQL_WLONGVARCHAR:
//	case SQL_GUID:
//	{
//	if( !ReadString( nColumn ) )
//	{
//	return false;
//	}
//	}
//	break;
//	case SQL_BIT:
//	{
//	bool val;
//	if( !GetQuery( )->GetStatement( ).GetData( nColumn + 1, SQL_C_BIT, &val, sizeof( bool ), &strLen_or_IndPtr ) )
//	{
//	return false;
//	}
//
//	if( strLen_or_IndPtr == SQL_NULL_DATA )
//	{
//	pColumn->SetNull( );
//	}
//	else
//	{
//	pColumn->SetBool( val );
//	}
//	}
//	break;
//	case SQL_SMALLINT:
//	case SQL_TINYINT:
//	case SQL_INTEGER:
//	{
//	long nValue;
//	if( !GetQuery( )->GetStatement( ).GetData( nColumn + 1, SQL_C_SLONG, &nValue, sizeof( long ), &strLen_or_IndPtr ) )
//	{
//	return false;
//	}
//
//	if( strLen_or_IndPtr == SQL_NULL_DATA )
//	{
//	pColumn->SetNull( );
//	}
//	else
//	{
//	pColumn->SetInt( nValue );
//	}
//	}
//	break;
//	case SQL_BIGINT:
//	{
//	__int64 val;
//
//	if( !GetQuery( )->GetStatement( ).GetData( nColumn + 1, SQL_C_SBIGINT, &val, sizeof( val ), &strLen_or_IndPtr ) )
//	{
//	return false;
//	}
//
//	if( strLen_or_IndPtr == SQL_NULL_DATA )
//	{
//	pColumn->SetNull( );
//	}
//	else
//	{
//	pColumn->SetInt64( val );
//	}
//	}
//	break;
//	case SQL_DECIMAL:
//	case SQL_NUMERIC:
//	{
//	SQL_NUMERIC_STRUCT numeric;
//
//	if( !GetQuery( )->GetStatement( ).GetData( nColumn + 1, SQL_C_NUMERIC, &numeric, sizeof( SQL_NUMERIC_STRUCT ), &strLen_or_IndPtr ) )
//	{
//	return false;
//	}
//
//	if( strLen_or_IndPtr == SQL_NULL_DATA )
//	{
//	pColumn->SetNull( );
//	}
//	else
//	{
//	pColumn->SetNumeric( numeric );
//	}
//	}
//	break;
//	case SQL_REAL:
//	case SQL_FLOAT:
//	case SQL_DOUBLE:
//	{
//	double val;
//
//	if( !GetQuery( )->GetStatement( ).GetData( nColumn + 1, SQL_C_DOUBLE, &val, sizeof( double ), &strLen_or_IndPtr ) )
//	{
//	return false;
//	}
//
//	if( strLen_or_IndPtr == SQL_NULL_DATA )
//	{
//	pColumn->SetNull( );
//	}
//	else
//	{
//	pColumn->SetDouble( val );
//	}
//	}
//	break;
//	case SQL_BINARY:
//	case SQL_VARBINARY:
//	case SQL_LONGVARBINARY:
//	{
//	if( pMeta->m_nColumnSize == 0 )
//	{
//	pColumn->SetNull( );
//	return true;
//	}
//
//	uint8_t* pBuffer = new uint8_t[ strLen_or_IndPtr ];
//
//	if( !GetQuery( )->GetStatement( ).GetData( nColumn + 1, SQL_C_BINARY, pBuffer, strLen_or_IndPtr, &strLen_or_IndPtr ) )
//	{
//	SafeDeleteArray( pBuffer );
//	return false;
//	}
//
//	if( strLen_or_IndPtr == SQL_NULL_DATA )
//	{
//	SafeDeleteArray( pBuffer );
//	pColumn->SetNull( );
//	}
//	else
//	{
//	pColumn->SetBuffer( pBuffer, strLen_or_IndPtr );
//	}
//	}
//	break;
//	case SQL_TYPE_TIMESTAMP:
//	case SQL_TYPE_DATE:
//
//	case SQL_SS_TIMESTAMPOFFSET:
//	{
//	__debugbreak( );
//	}
//	break;
//	case SQL_TYPE_TIME:
//	case SQL_SS_TIME2:
//	{
//	__debugbreak( );
//	}
//	break;
//	}
//	*/
//	return true;
//}
//
//bool CResultSet::ReadString( SQLUSMALLINT nColumn )
//{
//	/*	sColumnData* pColumn = &m_pActiveRow[ nColumn ];
//	MetaDefinition* pMeta = &m_pMetaData[ nColumn ];
//
//	SQLLEN display_size = pMeta->m_nColumnSize;
//
//	if( pMeta->m_nColumnSize == 0 )
//	{
//	pColumn->SetNull( );
//	return true;
//	}
//
//
//	SQLLEN value_len = 0;
//
//	if( pMeta->m_nDataType == SQL_CHAR || pMeta->m_nDataType == SQL_VARCHAR || pMeta->m_nDataType == SQL_LONGVARCHAR )
//	{
//	char* pBuffer = new char[ display_size + 1 ];
//
//	if( !GetQuery( )->GetStatement( ).GetData( nColumn + 1, SQL_C_CHAR, pBuffer, ( display_size + 1 ), &value_len ) )
//	{
//	SafeDeleteArray( pBuffer );
//	return false;
//	}
//
//	if( value_len == SQL_NO_DATA )
//	{
//	pColumn->SetNull( );
//	SafeDeleteArray( pBuffer );
//	}
//	else
//	{
//	pColumn->SetString( pBuffer, value_len );
//	}
//	}
//	else
//	{
//	wchar_t* pBuffer = new wchar_t[ display_size + 1 ];
//
//	if( !GetQuery( )->GetStatement( ).GetData( nColumn + 1, SQL_C_WCHAR, pBuffer, ( display_size + 1 ) * sizeof( wchar_t ), &value_len ) )
//	{
//	SafeDeleteArray( pBuffer );
//	return false;
//	}
//
//	if( value_len == SQL_NO_DATA )
//	{
//	pColumn->SetNull( );
//	SafeDeleteArray( pBuffer );
//	}
//	else
//	{
//	pColumn->SetString( pBuffer, value_len / sizeof( wchar_t ) );
//	}
//	}
//	*/
//
//	return true;
//}
