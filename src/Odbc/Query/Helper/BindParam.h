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
#include "JSValue.h"



enum class EInputOutputValue : SQLSMALLINT
{
	eInput = SQL_PARAM_INPUT,
	eOutput = SQL_PARAM_OUTPUT,
};


class CBindParam
{
public:
	CBindParam( );
	~CBindParam( );

public:
	void Dispose( );

public:
	inline void SetNull( )
	{
		SetData( SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, sizeof( char ), 0, nullptr, 0, SQL_NULL_DATA );
	}

	inline void SetBool( bool bValue )
	{
		m_data.bValue = bValue;

		SetPrimitve< SQLCHAR >( SQL_PARAM_INPUT, SQL_C_BIT, SQL_BIT, &m_data.bValue );
	}

	inline void SetInt32( int32_t nValue )
	{
		m_data.nInt32 = nValue;

		SetPrimitve< int32_t >( SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, &m_data.nInt32 );
	}

	inline void SetInt64( int64_t nValue )
	{
		m_data.nInt64 = nValue;

		SetPrimitve< int64_t >( SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, &m_data.nInt64 );
	}

	inline void SetDouble( double dValue )
	{
		m_data.dNumber = dValue;

		SetPrimitve< double >( SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE, &m_data.dNumber );
	}

	inline void SetString( v8::Local< v8::String > value )
	{
		int nLength = value->Length( );

		m_data.stringDesc.Alloc( EStringType::eUnicode, static_cast< size_t >( nLength ) );
		{
			value->Write( reinterpret_cast< uint16_t* >( m_data.stringDesc.data.pWString ) );
		}
		nLength *= sizeof( wchar_t );

		SetData( SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, nLength, 0, m_data.stringDesc.data.pWString, nLength, nLength );
	}

	inline void SetDate( int64_t ms )
	{
		CJSDate::ToSqlDate( ms, m_data.sqlDate );
		SetData( SQL_PARAM_INPUT, SQL_C_TYPE_DATE, SQL_TYPE_DATE, 0, 0, &m_data.sqlDate, sizeof( SQL_DATE_STRUCT ), sizeof( SQL_DATE_STRUCT ) );
	}

	inline void SetTimestamp( int64_t ms )
	{
		CJSDate::ToSqlTimestamp( ms, m_data.sqlTimestamp );
		SetData( SQL_PARAM_INPUT, SQL_C_TYPE_TIMESTAMP, SQL_TYPE_TIMESTAMP, 0, 3, &m_data.sqlTimestamp, sizeof( SQL_TIMESTAMP_STRUCT ), sizeof( SQL_TIMESTAMP_STRUCT ) );
	}

	inline void SetBuffer( v8::Local< v8::Value > value )
	{
		size_t nLength = node::Buffer::Length( value );
		const char* pBuffer = node::Buffer::Data( value );

		m_data.bufferDesc.m_nLength = nLength;
		m_data.bufferDesc.m_pBuffer = new uint8_t[ nLength ];
		{
			memcpy_s( m_data.bufferDesc.m_pBuffer, nLength, pBuffer, nLength );

			SetData( SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, nLength, 0, m_data.bufferDesc.m_pBuffer, static_cast< SQLLEN >( nLength ), static_cast< SQLLEN >( nLength ) );
		}
	}

public:
	void SetStream( v8::Isolate* isolate, v8::Local< v8::Object > value )
	{
		v8::HandleScope scope( isolate );
		const auto context = isolate->GetCurrentContext( );

		auto type = value->Get( context, Nan::New( "type" ).ToLocalChecked( ) ).ToLocalChecked( );
		auto stream = value->Get( context, Nan::New( "stream" ).ToLocalChecked( ) ).ToLocalChecked( );
		auto length = value->Get( context, Nan::New( "length" ).ToLocalChecked( ) ).ToLocalChecked( );

		ESqlType eType = static_cast< ESqlType >( type->Uint32Value( context ).FromJust( ) );
		uint32_t nLength = length->Uint32Value( context ).FromJust( );

		m_nDataWritten = 0;
		m_paramRef.Reset( isolate, stream.As< v8::Value >( ) );

		if( eType == ESqlType::eLongNVarChar )
		{
			SetData( SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WLONGVARCHAR, nLength, 0, ( SQLPOINTER )this, 0, SQL_DATA_AT_EXEC );
		}
		else
		{
			SetData( SQL_PARAM_INPUT, SQL_C_BINARY, SQL_LONGVARBINARY, nLength, 0, ( SQLPOINTER )this, 0, SQL_DATA_AT_EXEC );
		}		
	}

	void SetNumeric( v8::Isolate* isolate, v8::Local< v8::Object > value );

	bool SetOutputParameter( v8::Isolate* isolate, v8::Local< v8::Object > value );


public:
	void SetStringData( bool bInput, size_t nLength )
	{
		SetData( SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, nLength, 0, m_data.stringDesc.data.pWString, nLength, nLength );
	}



private:
	template< typename T >
	inline void SetPrimitve( SQLSMALLINT nInputOutputType, SQLSMALLINT nValueType, SQLSMALLINT nParameterType, SQLPOINTER pBuffer )
	{
		SetData( nInputOutputType, nValueType, nParameterType, sizeof( T ), 0, pBuffer, sizeof( T ), 0 );
	}

	inline void SetData( SQLSMALLINT nInputOutputType, SQLSMALLINT nValueType, SQLSMALLINT nParameterType, SQLUINTEGER nColumnSize, SQLSMALLINT nDigits, SQLPOINTER pBuffer, SQLLEN nBufferLen, SQLLEN strLen_or_IndPtr )
	{
		m_nInputOutputType = nInputOutputType;
		m_nValueType = nValueType;
		m_nParameterType = nParameterType;
		m_nColumnSize = nColumnSize;
		m_nDigits = nDigits;
		m_pBuffer = pBuffer;
		m_nBufferLen = nBufferLen;
		m_strLen_or_IndPtr = strLen_or_IndPtr;
	}

public:
	inline bool IsOutputParam( )
	{
		return m_nInputOutputType == SQL_PARAM_OUTPUT;
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

public:
	v8::Persistent< v8::Value, v8::CopyablePersistentTraits< v8::Value > >			m_paramRef;
};
