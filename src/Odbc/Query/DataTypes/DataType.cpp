#include "stdafx.h"
#include "DataType.h"

CDataType::CDataType( )
{

}

CDataType::~CDataType( )
{

}

bool CDataType::GetData( COdbcStatementHandle* pStatement, SSqlData* pData, size_t nColumn, SQLSMALLINT nTargetType, SQLPOINTER nTargetValue, SQLLEN nBufferLength, SQLLEN* pStrLen )
{
	SQLLEN strLen_or_IndPtr;

	if( !pStatement->GetDataEx( nColumn, nTargetType, nTargetValue, nBufferLength, &strLen_or_IndPtr ) )
	{
		return false;
	}

	if( strLen_or_IndPtr == SQL_NULL_DATA )
	{
		pData->SetNull( );
	}
	
	if( pStrLen != nullptr )
	{
		*pStrLen = strLen_or_IndPtr;
	}

	return true;
}
