// File: OdbcStatementHandle.h
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
extern std::atomic< size_t >	g_nSqlStatementCount;
#endif

class COdbcStatementHandle : public TOdbcHandle< eOdbcHandleType::eSqlHandleStmt >
{
public:
	COdbcStatementHandle( )
	{
#ifdef _DEBUG
		g_nSqlStatementCount++;
#endif
	}

	virtual ~COdbcStatementHandle( )
	{
#ifdef _DEBUG
		g_nSqlStatementCount--;
#endif
	}

public:
	inline const SQLRETURN ExecDirect( const std::wstring& szQuery )
	{
		return SQLExecDirectW(
			GetSqlHandle( ),
			const_cast< SQLWCHAR* >( szQuery.c_str( ) ),
			static_cast< SQLINTEGER >( szQuery.size( ) )
		);
	}

	inline const SQLRETURN ParamData( SQLPOINTER* pValue )
	{
		return SQLParamData( GetSqlHandle( ), pValue );
	}

	inline bool BindParameter( SQLUSMALLINT ParameterNumber, SQLSMALLINT InputOutputType, SQLSMALLINT ValueType, SQLSMALLINT ParameterType, SQLULEN ColumnSize, SQLSMALLINT DecimalDigits, SQLPOINTER ParameterValuePtr, SQLLEN BufferLength, SQLLEN* StrLen_or_IndPtr )
	{
		return VALIDATE_SQL_RESULT(
			SQLBindParameter(
				GetSqlHandle( ),
				ParameterNumber,
				InputOutputType,
				ValueType,
				ParameterType,
				ColumnSize,
				DecimalDigits,
				ParameterValuePtr,
				BufferLength,
				StrLen_or_IndPtr
			)
		);
	}

	inline bool SetStmtAttr( SQLINTEGER fAttribute, SQLPOINTER rgbValue, SQLINTEGER cbValueMax )
	{
		return VALIDATE_SQL_RESULT(
			SQLSetStmtAttrW( GetSqlHandle( ), fAttribute, rgbValue, cbValueMax )
		);
	}


	inline bool NumResultCols( SQLSMALLINT* nColumnCount )
	{
		return VALIDATE_SQL_RESULT( SQLNumResultCols( GetSqlHandle( ), nColumnCount ) );
	}

	inline SQLRETURN MoreResults( )
	{
		return SQLMoreResults( GetSqlHandle( ) );
	}

	inline bool DescribeCol( SQLUSMALLINT icol, SQLWCHAR* szColName, SQLSMALLINT cchColNameMax, SQLSMALLINT* pcchColName, SQLSMALLINT* pfSqlType, SQLULEN* pcbColDef, SQLSMALLINT* pibScale, SQLSMALLINT* pfNullable )
	{
		return VALIDATE_SQL_RESULT( SQLDescribeColW( GetSqlHandle( ), icol, szColName, cchColNameMax, pcchColName, pfSqlType, pcbColDef, pibScale, pfNullable ) );
	}

	inline bool DescribeCol( SQLUSMALLINT icol, std::wstring* szColName, SQLSMALLINT* pfSqlType, SQLULEN* pcbColDef, SQLSMALLINT* pibScale, SQLSMALLINT* pfNullable )
	{
		SQLSMALLINT nColNameLength = 0;

		if( !DescribeCol( icol, nullptr, 0, &nColNameLength, pfSqlType, pcbColDef, pibScale, pfNullable ) )
		{
			return false;
		}

		szColName->resize( static_cast< size_t >( nColNameLength ) );
		nColNameLength += 1;

		return DescribeCol( icol, &szColName->at( 0 ), nColNameLength, &nColNameLength, pfSqlType, pcbColDef, pibScale, pfNullable );
	}


	inline bool ColAttribute( SQLUSMALLINT iCol, SQLUSMALLINT iField, std::wstring* pCharAttr, SQLLEN* pNumAttr )
	{
		SQLSMALLINT nTextLength = 0;
		auto sqlRet = SQLColAttributeW( GetSqlHandle( ), iCol, iField, nullptr, 0, &nTextLength, pNumAttr );
		if( !SQL_SUCCEEDED( sqlRet ) )
		{
			return false;
		}

		pCharAttr->resize( static_cast< size_t >( nTextLength ) );
		nTextLength += 1;

		return VALIDATE_SQL_RESULT( SQLColAttributeW( GetSqlHandle( ), iCol, iField, &pCharAttr->at( 0 ), nTextLength, &nTextLength, pNumAttr ) );
	}

	inline SQLRETURN GetData( SQLUSMALLINT ColumnNumber, SQLSMALLINT TargetType, SQLPOINTER TargetValue, SQLLEN BufferLength, SQLLEN *StrLen_or_IndPtr )
	{
		return SQLGetData( GetSqlHandle( ), ColumnNumber, TargetType, TargetValue, BufferLength, StrLen_or_IndPtr );
	}

	inline SQLRETURN FetchScroll( SQLSMALLINT FetchOrientation, SQLLEN FetchOffset )
	{
		return SQLFetchScroll( GetSqlHandle( ), FetchOrientation, FetchOffset );
	}

};