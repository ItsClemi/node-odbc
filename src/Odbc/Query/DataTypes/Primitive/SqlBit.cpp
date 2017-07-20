#include "stdafx.h"
#include "SqlBit.h"


using namespace v8;



CSqlBit::CSqlBit( )
{ }

CSqlBit::~CSqlBit( )
{ }

void CSqlBit::Serialize( v8::Isolate* isolate, v8::Local<v8::Value> value, SSqlBindParam* pParam )
{
	HandleScope scope( isolate );
	const auto context = isolate->GetCurrentContext( );

	if( pParam->IsInputParam( ) )
	{
		pParam->GetData( )->bValue = value->BooleanValue( context ).FromJust( );
	}

	pParam->Set( SQL_C_BIT, SQL_BIT, sizeof( bool ), 0, &pParam->GetData( )->bValue, sizeof( bool ), NULL );
}

bool CSqlBit::Deserialize( COdbcStatementHandle* pStatement, size_t nColumn, SMetaData* pMetaData, SSqlData* pData )
{
	pData->m_eType = ESqlType::eBit;

	if( !GetData( pStatement, pData, nColumn, SQL_C_BIT, &pData->m_data.bValue, sizeof( bool ) ) )
	{
		return false;
	}

	return true;
}