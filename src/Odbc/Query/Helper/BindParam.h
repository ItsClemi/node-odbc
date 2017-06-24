// File: BindParam.h
// 
// node-odbc (odbc interface for NodeJS)
// 
// Copyright 2017 Clemens Susenbeth
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once


#include "ParamData.h"
#include "JSDate.h"


//shared with node-odbc.ts
enum eSqlOutputType : size_t
{
	eBitOutput,
	eTinyintOutput,
	eSmallint,
	eInt,
	eUint32,
	eBigInt,
	eFloat,
	eReal,
	eChar,
	eNChar,
	eVarChar,
	eNVarChar,
	eBinary,
	eVarBinary,
	eDate,
	eTimestamp,
	eNumeric,

	eMax,
};


class CBindParam
{
public:
	CBindParam( );
	~CBindParam( );

public:
	void Dispose( )
	{
		if( !m_stream.IsEmpty( ) )
		{
			assert( v8::Isolate::GetCurrent( ) != nullptr );
			m_stream.Reset( );
		}

		if( m_nParameterType == SQL_WVARCHAR )
		{
			delete[ ] m_data.stringDesc.m_stringData.pWString;
			//scalable_free( m_data.stringDesc.m_stringData.pWString );
		}
		else if( m_nParameterType == SQL_VARBINARY )
		{
			delete[ ] m_data.bufferDesc.m_pBuffer; 
			//scalable_free( m_data.bufferDesc.m_pBuffer );
		}
	}

public:
	inline void SetNull( )
	{
		SetData( SQL_C_CHAR, SQL_CHAR, sizeof( char ), 0, nullptr, 0, SQL_NULL_DATA );
	}

	inline void SetBool( bool bValue )
	{
		m_data.bValue = bValue;

		SetPrimitve< SQLCHAR >( SQL_C_BIT, SQL_BIT, &m_data.bValue );
	}

	inline void SetInt32( int32_t nValue )
	{
		m_data.nInt32 = nValue;

		SetPrimitve< int32_t >( SQL_C_SLONG, SQL_INTEGER, &m_data.nInt32 );
	}

	inline void SetUint32( uint32_t nValue )
	{
		m_data.nUint32 = nValue;

		SetPrimitve< uint32_t >( SQL_C_ULONG, SQL_BIGINT, &m_data.nUint32 );
	}

	inline void SetInt64( int64_t nValue )
	{
		m_data.nInt64 = nValue;

		SetPrimitve< int64_t >( SQL_C_SBIGINT, SQL_BIGINT, &m_data.nInt64 );
	}

	inline void SetDouble( double dValue )
	{
		m_data.dNumber = dValue;

		SetPrimitve< double >( SQL_C_DOUBLE, SQL_DOUBLE, &m_data.dNumber );
	}

	inline void SetString( v8::Local< v8::String > value )
	{
		int nLength = value->Length( );

		m_data.stringDesc.m_eType = EStringType::eUnicode;
		m_data.stringDesc.m_nLength = nLength;
		m_data.stringDesc.m_stringData.pWString = new wchar_t[ ( nLength + 1 ) ];
		//m_data.stringDesc.m_stringData.pWString = static_cast< wchar_t* >( scalable_malloc( ( nLength + 1 ) * sizeof( wchar_t ) ) );
		{
			value->Write( reinterpret_cast< uint16_t* >( m_data.stringDesc.m_stringData.pWString ) );
		}
		m_data.stringDesc.m_stringData.pWString[ nLength ] = '\0';

		nLength *= sizeof( wchar_t );

		SetData( SQL_C_WCHAR, SQL_WVARCHAR, nLength, 0, m_data.stringDesc.m_stringData.pWString, nLength, nLength );
	}

	inline void SetTimestamp( int64_t ms )
	{
		CJSDate::ToSqlTimestamp( ms, m_data.sqlTimestamp );
		SetData( SQL_C_TYPE_TIMESTAMP, SQL_TYPE_TIMESTAMP, 0, 3, &m_data.sqlTimestamp, sizeof( SQL_TIMESTAMP_STRUCT ), sizeof( SQL_TIMESTAMP_STRUCT ) );
	}

	inline void SetBuffer( v8::Local< v8::Value > value )
	{
		size_t nLength = node::Buffer::Length( value );
		const char* pBuffer = node::Buffer::Data( value );

		m_data.bufferDesc.m_nLength = nLength;
		//m_data.bufferDesc.m_pBuffer = static_cast< uint8_t* >( scalable_malloc( nLength ) );
		m_data.bufferDesc.m_pBuffer = new uint8_t[ nLength ];
		{
			memcpy_s( m_data.bufferDesc.m_pBuffer, nLength, pBuffer, nLength );

			SetData( SQL_C_BINARY, SQL_BINARY, nLength, 0, m_data.bufferDesc.m_pBuffer, static_cast< SQLLEN >( nLength ), static_cast< SQLLEN >( nLength ) );
		}
	}

public:
	bool SetStream( v8::Isolate* isolate, v8::Local< v8::Object > value );

	bool SetNumeric( v8::Isolate* isolate, v8::Local< v8::Object > value );

	bool SetDate( v8::Isolate* isolate, v8::Local< v8::Object > value );

	bool SetOutputParameter( v8::Isolate* isolate, v8::Local< v8::Object > value );

private:
	template< typename T >
	inline void SetPrimitve( SQLSMALLINT nValueType, SQLSMALLINT nParameterType, SQLPOINTER pBuffer )
	{
		SetData( nValueType, nParameterType, sizeof( T ), 0, pBuffer, sizeof( T ), sizeof( T ) );
	}

	inline void SetData( SQLSMALLINT nValueType, SQLSMALLINT nParameterType, SQLUINTEGER nColumnSize, SQLSMALLINT nDigits, SQLPOINTER pBuffer, SQLLEN nBufferLen, SQLLEN strLen_or_IndPtr )
	{
		m_nValueType = nValueType;
		m_nParameterType = nParameterType;
		m_nColumnSize = nColumnSize;
		m_nDigits = nDigits;
		m_pBuffer = pBuffer;
		m_nBufferLen = nBufferLen;
		m_strLen_or_IndPtr = strLen_or_IndPtr;
	}

public:
	SQLSMALLINT				m_nInputOutputType;
	SQLSMALLINT				m_nValueType;
	SQLSMALLINT				m_nParameterType;
	SQLUINTEGER				m_nColumnSize;
	SQLSMALLINT				m_nDigits;
	SQLPOINTER				m_pBuffer;
	SQLLEN					m_nBufferLen;
	SQLLEN					m_strLen_or_IndPtr;

private:
	SParamData				m_data;
	SQLUINTEGER				m_nDataWritten;


	v8::Persistent< v8::Value, v8::CopyablePersistentTraits< v8::Value > >			m_paramRef;
	v8::Persistent< v8::Object, v8::CopyablePersistentTraits< v8::Object > >		m_stream;
};
