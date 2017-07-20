#include "stdafx.h"
#include "SqlBinary.h"

using namespace v8;

CSqlBinary::CSqlBinary( )
{ }

CSqlBinary::~CSqlBinary( )
{ }

void CSqlBinary::Serialize( Isolate* isolate, Local< Value > value, SSqlBindParam* pParam )
{
	HandleScope scope( isolate );
	const auto context = isolate->GetCurrentContext( );

	const auto object = value.As< Object >( );

	auto dataBuffer = &pParam->GetData( )->dataBuffer;

	if( pParam->IsInputParam( ) )
	{
		size_t nLength = node::Buffer::Length( object );
		const char* pData = node::Buffer::Data( object );

		auto pBuffer = dataBuffer->AllocBuffer( nLength );
		memcpy_s( pBuffer, dataBuffer->length, pData, nLength );
	}
	else
	{
		auto length = object->Get( context, Nan::New( "length" ).ToLocalChecked( ) ).ToLocalChecked( );

		dataBuffer->AllocBuffer( length->Uint32Value( context ).FromJust( ) );
	}

	pParam->Set(
		SQL_C_BINARY,
		SQL_VARBINARY,
		static_cast< SQLUINTEGER >( dataBuffer->length ),
		0,
		dataBuffer->data.pBuffer,
		static_cast< SQLLEN >( dataBuffer->length ),
		static_cast< SQLLEN >( dataBuffer->length )
	);
}

bool CSqlBinary::Deserialize( COdbcStatementHandle* pStatement, size_t nColumn, SMetaData* pMetaData, SSqlData* pData )
{
	pData->m_eType = ESqlType::eVarBinary;


	SQLLEN nStringLength;
	//if( !GetData(pStatement, pData, nColumn, SQL_C_BINARY, ) )


}
