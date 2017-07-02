#include "stdafx.h"
#include "SqlBigInt.h"

using namespace v8;

CSqlBigInt::CSqlBigInt( )
{ }

CSqlBigInt::~CSqlBigInt( )
{ }

void CSqlBigInt::TransformType( Isolate* isolate, Local< Value > value, SSqlBindParam* pParam )
{
	HandleScope scope( isolate );
	const auto context = isolate->GetCurrentContext( );

	if( pParam->IsInputParam( ) )
	{
		pParam->GetData( )->nInt64 = value->IntegerValue( context ).FromJust( );
	}

	pParam->Set( SQL_C_SBIGINT, SQL_BIGINT, sizeof( int64_t ), 0, &pParam->GetData( )->nInt64, sizeof( int64_t ), 0 );
}

bool CSqlBigInt::TransformSqlType( COdbcStatementHandle* pStatement, size_t nColumn, SSqlData* pData )
{
	pData->m_eType = ESqlType::eBigInt;

	if( !GetData( pStatement, pData, nColumn, SQL_C_BINARY, &pData->m_data.nInt64, sizeof( int64_t ) ) )
	{
		return false;
	}
	
	return true;
}
