#include "stdafx.h"
#include "SqlReal.h"

using namespace v8;


CSqlReal::CSqlReal( )
{ }

CSqlReal::~CSqlReal( )
{ }

void CSqlReal::Serialize( Isolate* isolate, Local< Value > value, SSqlBindParam* pParam )
{
	HandleScope scope( isolate );
	const auto context = isolate->GetCurrentContext( );

	if( pParam->IsInputParam( ) )
	{
		pParam->GetData( )->dNumber = value->NumberValue( context ).FromJust( );
	}

	pParam->Set( SQL_C_DOUBLE, SQL_DOUBLE, sizeof( double ), 0, &pParam->GetData( )->dNumber, sizeof( double ), NULL );
}

bool CSqlReal::Deserialize( COdbcStatementHandle* pStatement, size_t nColumn, SMetaData* pMetaData, SSqlData* pData )
{
	pData->m_eType = ESqlType::eReal;

	if( !GetData( pStatement, pData, nColumn, SQL_C_DOUBLE, &pData->m_data.dNumber, sizeof( double ) ) )
	{
		return false;
	}

	return true;
}
