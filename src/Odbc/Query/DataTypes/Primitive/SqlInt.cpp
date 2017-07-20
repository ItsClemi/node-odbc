#include "stdafx.h"
#include "SqlInt.h"

using namespace v8;

CSqlInt::CSqlInt( )
{ }

CSqlInt::~CSqlInt( )
{ }

void CSqlInt::Serialize( v8::Isolate* isolate, Local< Value > value, SSqlBindParam* pParam )
{
	HandleScope scope( isolate );
	const auto context = isolate->GetCurrentContext( );

	SQLSMALLINT nDataType = SQL_INTEGER;

	if( pParam->IsInputParam( ) )
	{
		pParam->GetData( )->nInt32 = value->Int32Value( context ).FromJust( );
	}
	else
	{
		switch( pParam->GetOutputType( ) )
		{
			case ESqlType::eTinyint:
			{
				nDataType = SQL_TINYINT;
				break;
			}
			case ESqlType::eSmallint:
			{
				nDataType = SQL_SMALLINT;
				break;
			}
			case ESqlType::eInt32:
			{
				nDataType = SQL_INTEGER;
				break;
			}
#ifdef _DEBUG
			default:
			{
				__debugbreak( );
			}
#endif
		}
	}

	pParam->Set( SQL_C_SLONG, nDataType, sizeof( int32_t ), 0, &pParam->GetData( )->nInt32, sizeof( int32_t ), 0 );
}

bool CSqlInt::Deserialize( COdbcStatementHandle* pStatement, size_t nColumn, SMetaData* pMetaData, SSqlData* pData )
{
	pData->m_eType = ESqlType::eInt32;

	if( !GetData( pStatement, pData, nColumn, SQL_C_SLONG, &pData->m_data.nInt32, sizeof( int32_t ) ) )
	{
		return false;
	}

	return true;
}
