#include "stdafx.h"
#include "SqlDate.h"


using namespace v8;


CSqlDate::CSqlDate( )
{

}

CSqlDate::~CSqlDate( )
{

}

void CSqlDate::Serialize( Isolate* isolate, Local< Value > value, SSqlBindParam* pParam )
{
	HandleScope scope( isolate );
	const auto context = isolate->GetCurrentContext( );

	if( pParam->IsInputParam( ) )
	{
		CJSDate::ToSqlDate( value->IntegerValue( context ).FromJust( ), pParam->GetData( )->sqlDate );
	}

	pParam->Set( SQL_C_TYPE_DATE, SQL_TYPE_DATE, 0, 0, &pParam->GetData( )->sqlDate, sizeof( SQL_DATE_STRUCT ), sizeof( SQL_DATE_STRUCT ) );
}

bool CSqlDate::Deserialize( COdbcStatementHandle* pStatement, size_t nColumn, SMetaData* pMetaData, SSqlData* pData )
{
	if( !GetData( pStatement, pData, nColumn, SQL_C_DATE, &pData->m_data.sqlDate, sizeof( SQL_DATE_STRUCT ) ) )
	{
		return false;
	}

	return true;
}
