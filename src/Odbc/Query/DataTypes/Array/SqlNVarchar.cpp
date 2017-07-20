#include "stdafx.h"
#include "SqlNVarchar.h"

using namespace v8;


CSqlNVarchar::CSqlNVarchar( )
{ }

CSqlNVarchar::~CSqlNVarchar( )
{ }

void CSqlNVarchar::Serialize( Isolate* isolate, Local< Value > value, SSqlBindParam* pParam )
{
	HandleScope scope( isolate );
	const auto context = isolate->GetCurrentContext( );


	SQLUINTEGER nStringLength = 0;

	if( pParam->IsInputParam( ) )
	{
		auto string = value.As< String >( );

		wchar_t* pBuffer = pParam->GetData( )->dataBuffer.AllocWstring( string->Length( ) );

		string->Write( reinterpret_cast< uint16_t* >( pBuffer ) );
	}
	else
	{
		auto length = value.As< Object >( )->Get( context, Nan::New( "length" ).ToLocalChecked( ) ).ToLocalChecked( );

		pParam->GetData( )->dataBuffer.AllocWstring( length->Uint32Value( context ).FromJust( ) );
	}

	SDataBuffer* pData = &pParam->GetData( )->dataBuffer;

	pParam->Set(
		SQL_C_WCHAR,
		SQL_WVARCHAR,
		static_cast< SQLUINTEGER >( pData->length ),
		0,
		pData->data.pWstring,
		static_cast< SQLLEN >( pData->length ),
		0
	);
}

bool CSqlNVarchar::Deserialize( COdbcStatementHandle* pStatement, size_t nColumn, SMetaData* pMetaData, SSqlData* pData )
{
	pData->m_eType = ESqlType::eNVarChar;

	auto pBuffer = &pData->m_data.dataBuffer;

	pBuffer->AllocWstring( pMetaData->m_nColumnSize );

	SQLLEN nStringLength;
	if( !GetData( pStatement, pData, nColumn, SQL_C_WCHAR, pBuffer->data.pWstring, pBuffer->length, &nStringLength ) )
	{
		return false;
	}

	if( nStringLength == SQL_NULL_DATA )
	{
		pBuffer->DestroyWString( );
	}

	return true;
}

