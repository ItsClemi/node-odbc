#include "stdafx.h"
#include "SqlNumeric.h"


using namespace v8;


CSqlNumeric::CSqlNumeric( )
{ }

CSqlNumeric::~CSqlNumeric( )
{ }

void CSqlNumeric::Serialize( Isolate* isolate, Local< Value > value, SSqlBindParam* pParam )
{
	HandleScope scope( isolate );
	const auto context = isolate->GetCurrentContext( );

	const auto object = value.As< Object >( );

	const auto precision = object->Get( context, Nan::New( "precision" ).ToLocalChecked( ) ).ToLocalChecked( );
	const auto scale = object->Get( context, Nan::New( "scale" ).ToLocalChecked( ) ).ToLocalChecked( );

	auto sqlNumeric = &pParam->GetData( )->sqlNumeric;
	{
		sqlNumeric->precision = static_cast< SQLCHAR >( precision->Uint32Value( context ).FromJust( ) );
		sqlNumeric->scale = static_cast< SQLSCHAR >( scale->Uint32Value( context ).FromJust( ) );
	}

	SQLSMALLINT nDigits = 0;
	if( pParam->IsInputParam( ) )
	{
		const auto sign = object->Get( context, Nan::New( "sign" ).ToLocalChecked( ) ).ToLocalChecked( );
		const auto buffer = object->Get( context, Nan::New( "value" ).ToLocalChecked( ) ).ToLocalChecked( );
		const auto content = buffer.As< Uint8Array >( )->Buffer( )->GetContents( );

		sqlNumeric->sign = sign->BooleanValue( context ).FromJust( ) ? 1 : 0;
		
		memset( sqlNumeric->val, 0, SQL_MAX_NUMERIC_LEN );
		memcpy_s( sqlNumeric->val, SQL_MAX_NUMERIC_LEN, content.Data( ), content.ByteLength( ) );

		nDigits = static_cast< SQLSMALLINT >( content.ByteLength( ) );
	}

	pParam->Set(
		SQL_C_NUMERIC,
		SQL_NUMERIC,
		sqlNumeric->precision,
		nDigits,
		sqlNumeric,
		sizeof( SQL_NUMERIC_STRUCT ),
		sizeof( SQL_NUMERIC_STRUCT )
	);
}

bool CSqlNumeric::Deserialize( COdbcStatementHandle* pStatement, size_t nColumn, SMetaData* pMetaData, SSqlData* pData )
{
	pData->m_eType = ESqlType::eNumeric;

	if( !GetData( pStatement, pData, nColumn, SQL_C_NUMERIC, &pData->m_data.sqlNumeric, sizeof( SQL_NUMERIC_STRUCT ) ) )
	{
		return false;
	}

	return true;
}
