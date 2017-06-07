// File: ColumnData.h
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

enum class EJSType
{
	eNull,
	eString,
	eBoolean,
	eInt32,
	eInt64,
	eDouble,

	eNumeric,
	eDate,
	eTimestamp,
	eBuffer,
	eReadStream
};


struct SColumnData
{
	void Dispose( )
	{
		if( m_eType == EJSType::eString )
		{
			if( m_data.stringDesc.IsAnsiString( ) )
			{
				delete[ ] m_data.stringDesc.m_stringData.pString;
				//scalable_free( m_data.stringDesc.m_stringData.pString );
			}
			else
			{
				delete[ ] m_data.stringDesc.m_stringData.pWString;

				//scalable_free( m_data.stringDesc.m_stringData.pWString );
			}

		}
		else if( m_eType == EJSType::eBuffer )
		{
			delete[ ] m_data.bufferDesc.m_pBuffer;
			//scalable_free( m_data.bufferDesc.m_pBuffer );
		}
	}

	void SetNull( )
	{
		m_eType = EJSType::eNull;
	}

	void SetBool( bool bValue )
	{
		m_eType = EJSType::eBoolean;
		m_data.bValue = bValue;
	}

	void SetInt32( int32_t nInt32 )
	{
		m_eType = EJSType::eInt32;
		m_data.nInt32 = nInt32;
	}

	void SetInt64( int64_t nInt64 )
	{
		m_eType = EJSType::eInt64;
		m_data.nInt64 = nInt64;
	}

	void SetDouble( double dValue )
	{
		m_eType = EJSType::eDouble;
		m_data.dNumber = dValue;
	}

	void SetDate( SQL_DATE_STRUCT& sqlDate )
	{
		m_eType = EJSType::eDate;
		m_data.sqlDate = sqlDate;
	}

	void SetTimestamp( SQL_TIMESTAMP_STRUCT& sqlTimestamp )
	{
		m_eType = EJSType::eTimestamp;
		m_data.sqlTimestamp = sqlTimestamp;
	}

	void SetNumeric( SQL_NUMERIC_STRUCT& sqlNumeric )
	{
		m_eType = EJSType::eNumeric;
		m_data.sqlNumeric = sqlNumeric;
	}



	void SetString( char* pBuffer, size_t nLength )
	{
		m_eType = EJSType::eString;
		m_data.stringDesc.SetString( pBuffer, nLength );
	}

	void SetString( wchar_t* pBuffer, size_t nLength )
	{
		m_eType = EJSType::eString;
		m_data.stringDesc.SetString( pBuffer, nLength );
	}

	void SetBuffer( uint8_t* pBuffer, size_t nLength )
	{
		m_eType = EJSType::eBuffer;
		m_data.bufferDesc.SetBuffer( pBuffer, nLength );
	}

	void SetLobColumn( )
	{
		m_eType = EJSType::eReadStream;
	}

public:
	v8::Local< v8::Value > ToValue( v8::Isolate* isolate )
	{
		v8::EscapableHandleScope scope( isolate );
		const auto context = isolate->GetCurrentContext( );


		v8::Local< v8::Value > value;
		switch( m_eType )
		{
			case EJSType::eNull:		value = v8::Null( isolate );													break;
			case EJSType::eBoolean:		value = v8::Boolean::New( isolate, m_data.bValue );								break;
			case EJSType::eInt32:		value = v8::Int32::New( isolate, m_data.nInt32 );								break;
			case EJSType::eInt64:		value = v8::Number::New( isolate, static_cast< double >( m_data.nInt64 ) );		break;
			case EJSType::eDouble:		value = v8::Number::New( isolate, m_data.dNumber );								break;
			case EJSType::eDate:		value = CJSDate::ToMilliseconds( isolate, m_data.sqlDate );						break;
			case EJSType::eTimestamp:	value = CJSDate::ToMilliseconds( isolate, m_data.sqlTimestamp );				break;

			case EJSType::eString:
			{
				v8::MaybeLocal< v8::String > string;

				if( m_data.stringDesc.IsAnsiString( ) )
				{
					string = v8::String::NewFromUtf8( isolate, m_data.stringDesc.m_stringData.pString );
				}
				else
				{
					string = v8::String::NewFromTwoByte( 
						isolate,
						reinterpret_cast< const uint16_t* >( m_data.stringDesc.m_stringData.pWString ),
						v8::NewStringType::kNormal
					);
				}

				value = string.ToLocalChecked( );
				break;
			}

			case EJSType::eNumeric:
			{
				v8::Local< v8::Object > numeric = v8::Object::New( isolate );

				auto buffer = v8::ArrayBuffer::New( isolate, SQL_MAX_NUMERIC_LEN );
				auto contents = buffer->GetContents( );

				memcpy_s( contents.Data( ), contents.ByteLength( ), m_data.sqlNumeric.val, SQL_MAX_NUMERIC_LEN );

				if( numeric->Set( context, Nan::New( "precision" ).ToLocalChecked( ), v8::Int32::New( isolate, m_data.sqlNumeric.precision ) ).IsNothing( ) ||
					numeric->Set( context, Nan::New( "scale" ).ToLocalChecked( ), v8::Int32::New( isolate, m_data.sqlNumeric.scale ) ).IsNothing( ) ||
					numeric->Set( context, Nan::New( "sign" ).ToLocalChecked( ), v8::Boolean::New( isolate, m_data.sqlNumeric.sign ) ).IsNothing( ) ||
					numeric->Set( context, Nan::New( "value" ).ToLocalChecked( ), v8::Uint8Array::New( buffer, 0, SQL_MAX_NUMERIC_LEN ) ).IsNothing( )
					)
				{
					value = v8::Null( isolate );
				}
				else
				{
					value = numeric;
				}

				break;
			}

			case EJSType::eBuffer:
				value = node::Buffer::New( isolate, reinterpret_cast< char* >( m_data.bufferDesc.m_pBuffer ), m_data.bufferDesc.m_nLength ).ToLocalChecked( );
				break;

			case EJSType::eReadStream:
				assert( false );
				break;
			default:
				assert( false );
		}

		return scope.Escape( value );
	}


private:
	EJSType			m_eType;
	SParamData		m_data;
};