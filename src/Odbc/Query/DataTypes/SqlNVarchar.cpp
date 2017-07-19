#include "stdafx.h"
#include "SqlNVarchar.h"

using namespace v8;


CSqlNVarchar::CSqlNVarchar( )
{

}

CSqlNVarchar::~CSqlNVarchar( )
{

}

void CSqlNVarchar::TransformType( Isolate* isolate, Local< Value > value, SSqlBindParam* pParam )
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

bool CSqlNVarchar::TransformSqlType( COdbcStatementHandle* pStatement, size_t nColumn, SSqlData* pData )
{
	pData->m_eType = ESqlType::eNVarChar;

	SQLLEN nStringLength;
	if( !GetData( pStatement, pData, nColumn, SQL_C_WCHAR, pData->m_data.stringDesc.GetWString( ), pData->m_data.stringDesc.m_nLength, &nStringLength ) )
	{
		return false;
	}

	if( nStringLength == SQL_NULL_DATA )
	{
		pData->m_data.stringDesc.Dispose( );
	}

	return true;
}

