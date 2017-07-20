#include "stdafx.h"
#include "SqlTimestamp.h"


using namespace v8;


CSqlTimestamp::CSqlTimestamp( )
{ }

CSqlTimestamp::~CSqlTimestamp( )
{ }

void CSqlTimestamp::Serialize( Isolate* isolate, Local< Value > value, SSqlBindParam* pParam )
{
	HandleScope scope( isolate );
	const auto context = isolate->GetCurrentContext( );


	SQLSMALLINT nScale = 3;
	if( pParam->IsInputParam( ) )
	{
		CJSDate::ToSqlTimestamp( value->IntegerValue( context ).FromJust( ), pParam->GetData( )->sqlTimestamp );
	}
	else
	{
		const auto object = value.As< Object >( );

		auto scale = object->Get( context, Nan::New( "scale" ).ToLocalChecked( ) ).ToLocalChecked( );

		nScale = static_cast< SQLSMALLINT >( scale->IntegerValue( context ).FromJust( ) );
	}

	
	pParam->Set(
		SQL_C_TIMESTAMP,
		SQL_TYPE_TIMESTAMP,
		0,
		static_cast< SQLSMALLINT >( nScale ),
		&pParam->GetData()->sqlTimestamp, 
		sizeof( SQL_TIMESTAMP_STRUCT ),
		sizeof( SQL_TIMESTAMP_STRUCT )
	);
}

bool CSqlTimestamp::Deserialize( COdbcStatementHandle* pStatement, size_t nColumn, SMetaData* pMetaData, SSqlData* pData )
{
	if( !GetData( pStatement, pData, nColumn, SQL_C_TIMESTAMP, &pData->m_data.sqlTimestamp, sizeof( SQL_DATE_STRUCT ) ) )
	{
		return false;
	}

	return true;
}

