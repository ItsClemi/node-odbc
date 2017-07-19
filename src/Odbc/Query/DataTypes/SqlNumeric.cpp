#include "stdafx.h"
#include "SqlNumeric.h"


using namespace v8;


CSqlNumeric::CSqlNumeric( )
{ }

CSqlNumeric::~CSqlNumeric( )
{ }

//Serialize
void CSqlNumeric::TransformType( Isolate* isolate, Local< Value > value, SSqlBindParam* pParam )
{
	HandleScope scope( isolate );
	const auto context = isolate->GetCurrentContext( );

	const auto object = value.As< Object >( );

	auto precision = object->Get( context, Nan::New( "precision" ).ToLocalChecked( ) ).ToLocalChecked( );
	auto scale = object->Get( context, Nan::New( "scale" ).ToLocalChecked( ) ).ToLocalChecked( );
	auto sign = object->Get( context, Nan::New( "sign" ).ToLocalChecked( ) ).ToLocalChecked( );
	auto buffer = object->Get( context, Nan::New( "value" ).ToLocalChecked( ) ).ToLocalChecked( );


	if( pParam->IsInputParam( ) )
	{

	}
	else
	{

	}

	pParam->Set(
		SQL_C_NUMERIC,
		SQL_NUMERIC,
		pParam->GetData( )->sqlNumeric.precision,
		pParam->GetData( )->sqlNumeric.scale,
		&pParam->GetData( )->sqlNumeric,
		sizeof( SQL_NUMERIC_STRUCT ),
		sizeof( SQL_NUMERIC_STRUCT )
	);
}

//> Deseriealize
bool CSqlNumeric::TransformSqlType( COdbcStatementHandle* pStatement, size_t nColumn, SSqlData* pData )
{
	pData->m_eType = ESqlType::eNumeric;

	if( !GetData( pStatement, pData, nColumn, SQL_C_NUMERIC, &pData->m_data.sqlNumeric, sizeof( SQL_NUMERIC_STRUCT ) ) )
	{
		return false;
	}

	return true;
}
