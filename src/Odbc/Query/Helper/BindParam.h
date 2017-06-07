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



class CBindParam
{
public:
	CBindParam( )
	{

	}

	~CBindParam( )
	{
		assert( v8::Isolate::GetCurrent( ) != nullptr );
		assert( m_stream.IsEmpty( ) );
	}

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
			delete[ ]m_data.stringDesc.m_stringData.pWString;
			//scalable_free( m_data.stringDesc.m_stringData.pWString );
		}
		else if( m_nParameterType == SQL_VARBINARY )
		{
			delete[ ]m_data.bufferDesc.m_pBuffer; //scalable_free( m_data.bufferDesc.m_pBuffer );
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

		//SetData( SQL_C_BIT, SQL_BIT, sizeof( SQLCHAR ), 0, &m_data.bValue, sizeof( uint16_t ), sizeof( uint16_t ) );
	}

	inline void SetInt32( int32_t nValue )
	{
		m_data.nInt32 = nValue;

		SetPrimitve< int32_t >( SQL_C_SLONG, SQL_INTEGER, &m_data.nInt32 );


		//SetData( SQL_C_SLONG, SQL_INTEGER, sizeof( int32_t ), 0, &m_data.nInt32, sizeof( int32_t ), sizeof( int32_t ) );
	}

	inline void SetUint32( uint32_t nValue )
	{
		m_data.nUint32 = nValue;

		SetPrimitve< uint32_t >( SQL_C_ULONG, SQL_BIGINT, &m_data.nUint32 );

		//SetData( SQL_C_ULONG, SQL_BIGINT, sizeof( uint32_t ), 0, &m_data.unValue, sizeof( uint32_t ), sizeof( uint32_t ) );
	}

	inline void SetInt64( int64_t nValue )
	{
		m_data.nInt64 = nValue;

		SetPrimitve< int64_t >( SQL_C_SBIGINT, SQL_BIGINT, &m_data.nInt64 );

		//SetData( SQL_C_SBIGINT, SQL_BIGINT, sizeof( int64_t ), 0, &m_data.llValue, sizeof( int64_t ), sizeof( int64_t ) );
	}

	inline void SetDouble( double dValue )
	{
		m_data.dNumber = dValue;

		SetPrimitve< double >( SQL_C_DOUBLE, SQL_DOUBLE, &m_data.dNumber );

		//SetData( SQL_C_DOUBLE, SQL_DOUBLE, sizeof( double ), 0, &m_data.dValue, sizeof( double ), sizeof( double ) );
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
	inline bool SetStream( v8::Isolate* isolate, v8::Local< v8::Object > value )
	{
		v8::HandleScope scope( isolate );
		const auto context = isolate->GetCurrentContext( );

		auto _stream = value->Get( context, Nan::New( "stream" ).ToLocalChecked( ) );
		auto _length = value->Get( context, Nan::New( "length" ).ToLocalChecked( ) );

		if( _stream.IsEmpty( ) || _length.IsEmpty( ) )
		{
			return false;
		}

		auto stream = _stream.ToLocalChecked( );
		auto length = _length.ToLocalChecked( );

		if( !stream->IsObject( ) || !length->IsUint32( ) )
		{
			return false;
		}

		uint32_t nLength = length->Uint32Value( context ).FromJust( );

		m_nDataWritten = 0;
		//m_stream.Reset( isolate, stream.As< v8::Value >( ) );
		
		SetData( SQL_C_BINARY, SQL_LONGVARBINARY, nLength, 0, ( SQLPOINTER )this, 0, SQL_DATA_AT_EXEC );

		return true;
	}


	inline bool SetNumeric( v8::Isolate* isolate, v8::Local< v8::Object > value )
	{
		v8::HandleScope scope( isolate );
		const auto context = isolate->GetCurrentContext( );

		auto _precision = value->Get( context, Nan::New( "precision" ).ToLocalChecked( ) );
		auto _scale = value->Get( context, Nan::New( "scale" ).ToLocalChecked( ) );
		auto _sign = value->Get( context, Nan::New( "sign" ).ToLocalChecked( ) );
		auto _buffer = value->Get( context, Nan::New( "value" ).ToLocalChecked( ) );

		if( _precision.IsEmpty( ) || _scale.IsEmpty( ) || _sign.IsEmpty( ) || _buffer.IsEmpty( ) )
		{
			return false;
		}

		auto precision = _precision.ToLocalChecked( );
		auto scale = _scale.ToLocalChecked( );
		auto sign = _sign.ToLocalChecked( );
		auto buffer = _buffer.ToLocalChecked( );

		if( !precision->IsUint32( ) || !scale->IsUint32( ) || !sign->IsBoolean( ) || !buffer->IsUint8Array( ) )
		{
			return false;
		}

		auto nPrecision = precision.As< v8::Uint32 >( )->Uint32Value( context ).FromJust( );
		auto nScale = scale.As< v8::Uint32 >( )->Uint32Value( context ).FromJust( );
		auto bSign = sign.As< v8::Boolean >( )->BooleanValue( context ).FromJust( );
		auto contents = buffer.As< v8::Uint8Array >( )->Buffer( )->GetContents( );

		if( nPrecision > 255 || nScale > 127 || contents.ByteLength( ) > SQL_MAX_NUMERIC_LEN )
		{
			return false;
		}


		size_t nDigits = contents.ByteLength( );
		{
			memset( &m_data.sqlNumeric.val, 0, SQL_MAX_NUMERIC_LEN );
			m_data.sqlNumeric.precision = static_cast< SQLCHAR >( nPrecision );
			m_data.sqlNumeric.scale = static_cast< SQLSCHAR >( nScale );
			m_data.sqlNumeric.sign = static_cast< SQLCHAR >( bSign ? 1 : 2 );
			memcpy_s( m_data.sqlNumeric.val, SQL_MAX_NUMERIC_LEN, contents.Data( ), nDigits );
		}

		SetData( SQL_C_NUMERIC, SQL_NUMERIC, m_data.sqlNumeric.precision, static_cast< SQLSMALLINT >( nDigits ), &m_data.sqlNumeric, sizeof( SQL_NUMERIC_STRUCT ), sizeof( SQL_NUMERIC_STRUCT ) );

		return true;
	}

	inline bool SetDate( v8::Isolate* isolate, v8::Local< v8::Object > value )
	{
		v8::HandleScope scope( isolate );
		const auto context = isolate->GetCurrentContext( );

		auto _date = value->Get( context, Nan::New( "date" ).ToLocalChecked( ) );
		if( _date.IsEmpty( ) )
		{
			return false;
		}

		auto date = _date.ToLocalChecked( );

		if( !date->IsDate( ) )
		{
			return false;
		}
		

		auto ms = date.As< v8::Date >( )->IntegerValue( context ).FromJust( );

		{
			CJSDate::ToSqlDate( ms, m_data.sqlDate );
		}

		SetData( SQL_C_TYPE_DATE, SQL_TYPE_DATE, 0, 0, &m_data.sqlDate, sizeof( SQL_DATE_STRUCT ), sizeof( SQL_DATE_STRUCT ) );
		return true;
	}

private:
	template< typename T >
	inline void SetPrimitve( SQLSMALLINT nValueType, SQLSMALLINT nParameterType, SQLPOINTER pBuffer ) const
	{
		SetData( nValueType, nParameterType, sizeof( T ), 0, pBuffer, sizeof( T ), sizeof( T ) );
	}

	inline void SetData( SQLSMALLINT nValueType, SQLSMALLINT nParameterType, SQLUINTEGER nColumnSize, SQLSMALLINT nDigits, SQLPOINTER pBuffer, SQLLEN nBufferLen, SQLLEN strLen_or_IndPtr ) const
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
	mutable SQLSMALLINT				m_nValueType;
	mutable SQLSMALLINT				m_nParameterType;
	mutable SQLUINTEGER				m_nColumnSize;
	mutable SQLSMALLINT				m_nDigits;
	mutable SQLPOINTER				m_pBuffer;
	mutable SQLLEN					m_nBufferLen;
	mutable SQLLEN					m_strLen_or_IndPtr;

private:
	SParamData							m_data;

	SQLUINTEGER							m_nDataWritten;

	v8::Persistent< v8::Object, v8::CopyablePersistentTraits< v8::Object > >		m_stream;

};