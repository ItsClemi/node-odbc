#include "stdafx.h"
#include "SqlVarchar.h"

using namespace v8;

CSqlVarChar::CSqlVarChar( )
{ }

CSqlVarChar::~CSqlVarChar( )
{ }

void CSqlVarChar::Serialize( Isolate* isolate, Local< Value > value, SSqlBindParam* pParam )
{
	HandleScope scope( isolate );
	const auto context = isolate->GetCurrentContext( );

	assert( !pParam->IsInputParam( ) );


	auto length = value.As< Object >( )->Get( context, Nan::New( "length" ).ToLocalChecked( ) ).ToLocalChecked( );

	SDataBuffer* pData = &pParam->GetData( )->dataBuffer;
	pData->AllocBuffer( length->Uint32Value( context ).FromJust( ) );

	pParam->Set(
		SQL_C_CHAR,
		SQL_VARCHAR,
		static_cast< SQLUINTEGER >( pData->length ),
		0,
		pData->data.pString,
		static_cast< SQLLEN >( pData->length ),
		0
	);
}

bool CSqlVarChar::Deserialize( COdbcStatementHandle* pStatement, size_t nColumn, SMetaData* pMetaData, SSqlData* pData )
{
	pData->m_eType = ESqlType::eVarChar;

	auto pBuffer = &pData->m_data.dataBuffer;

	pBuffer->AllocString( pMetaData->m_nColumnSize );

	SQLLEN nStringLength;
	if( !GetData( pStatement, pData, nColumn, SQL_C_WCHAR, pBuffer->data.pString, pBuffer->length, &nStringLength ) )
	{
		return false;
	}

	if( nStringLength == SQL_NULL_DATA )
	{
		pBuffer->DestroyWString( );
	}

	return true;
}
