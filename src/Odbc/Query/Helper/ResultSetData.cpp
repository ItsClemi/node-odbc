#include "stdafx.h"
#include "ResultSetData.h"

#include "Odbc/Query/Query.h"



CResultSetData::CResultSetData( std::shared_ptr< CQuery > pQuery )
	:m_pQuery( pQuery )
{ }

CResultSetData::~CResultSetData( )
{

}

bool CResultSetData::PrepareColumns( size_t nColumns )
{
	m_vecMetaData.resize( nColumns );

	for( size_t i = 0; i < nColumns; i++ )
	{
		auto pMetaData = &m_vecMetaData[ i ];

		pMetaData->m_bLOBColumn = false;

		if( !m_pQuery->DescribeCol( 
			static_cast< SQLUSMALLINT >( i + 1 ), 
			&pMetaData->m_szColumnName, 
			&pMetaData->m_nDataType, 
			&pMetaData->m_nColumnSize, 
			&pMetaData->m_nDecimalDigits, 
			&pMetaData->m_bNullable 
		) )
		{
			return false;
		}

		switch( pMetaData->m_nDataType )
		{
			case SQL_VARCHAR:
			case SQL_LONGVARCHAR:
			case SQL_WCHAR:
			case SQL_WVARCHAR:
			case SQL_WLONGVARCHAR:
			case SQL_VARBINARY:
			case SQL_LONGVARBINARY:
			{
				if( pMetaData->m_nColumnSize == 0 )
				{
					pMetaData->m_bLOBColumn = true;
					m_bHasLobColumns = true;
				}
			}
		}

		if( m_pQuery->IsMetaDataEnabled( ) )
		{
			if( !m_pQuery->ColAttribute( 
				static_cast< SQLUSMALLINT >( i + 1 ), 
				SQL_DESC_TYPE_NAME, 
				&pMetaData->m_szDataTypeName, 
				nullptr 
			) )
			{
				return false;
			}
		}
	}


	return true;
}

bool CResultSetData::ReadColumn( size_t i )
{
	auto pMetaData = &m_vecMetaData[ i ];

	switch( pMetaData->m_nDataType )
	{
		case SQL_CHAR:
		case SQL_VARCHAR:
		case SQL_LONGVARCHAR:
		case SQL_WCHAR:
		case SQL_WVARCHAR:
		case SQL_WLONGVARCHAR:
		case SQL_GUID:
		{

			break;
		}
		case SQL_BIT:
		{
			break;
		}
		case SQL_TINYINT:
		case SQL_SMALLINT:
		case SQL_INTEGER:
		{
			break;
		}
		case SQL_BIGINT:
		{
			break;
		}
		case SQL_DECIMAL:
		case SQL_NUMERIC:
		{
			break;

		}
		case SQL_REAL:
		case SQL_FLOAT:
		case SQL_DOUBLE:
		{
			break;

		}
		case SQL_BINARY:
		case SQL_VARBINARY:
		case SQL_LONGVARBINARY:
		{
			break;

		}
		case SQL_TYPE_TIMESTAMP:
		case SQL_TYPE_DATE:
		case SQL_TYPE_TIME:
		case SQL_DATETIME:
		{
			break;
		}


		default:
		{

			return false;
		}
			
	}


	return true;
}
