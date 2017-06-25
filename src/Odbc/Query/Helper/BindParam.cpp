// File: BindParam.cpp
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

#include "stdafx.h"
#include "BindParam.h"


using namespace v8;



CBindParam::CBindParam( )
{
}

CBindParam::~CBindParam( )
{
	assert( v8::Isolate::GetCurrent( ) != nullptr );
	assert( m_paramRef.IsEmpty( ) );
	assert( m_stream.IsEmpty( ) );
}

bool CBindParam::SetStream( Isolate* isolate, Local< Object > value )
{
	HandleScope scope( isolate );
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

	SetData( SQL_PARAM_INPUT, SQL_C_BINARY, SQL_LONGVARBINARY, nLength, 0, ( SQLPOINTER )this, 0, SQL_DATA_AT_EXEC );

	return true;
}

bool CBindParam::SetNumeric( Isolate* isolate, Local< Object > value )
{
	HandleScope scope( isolate );
	const auto context = isolate->GetCurrentContext( );

	auto precision = value->Get( context, Nan::New( "precision" ).ToLocalChecked( ) ).ToLocalChecked( );
	auto scale = value->Get( context, Nan::New( "scale" ).ToLocalChecked( ) ).ToLocalChecked( );
	auto sign = value->Get( context, Nan::New( "sign" ).ToLocalChecked( ) ).ToLocalChecked( );
	auto buffer = value->Get( context, Nan::New( "value" ).ToLocalChecked( ) ).ToLocalChecked( );

	auto nPrecision = precision.As< Uint32 >( )->Uint32Value( context ).FromJust( );
	auto nScale = scale.As< Uint32 >( )->Uint32Value( context ).FromJust( );
	auto bSign = sign.As< Boolean >( )->BooleanValue( context ).FromJust( );
	auto contents = buffer.As< Uint8Array >( )->Buffer( )->GetContents( );

	size_t nDigits = contents.ByteLength( );
	{
		memset( &m_data.sqlNumeric.val, 0, SQL_MAX_NUMERIC_LEN );

		m_data.sqlNumeric.precision = static_cast< SQLCHAR >( nPrecision );
		m_data.sqlNumeric.scale = static_cast< SQLSCHAR >( nScale );
		m_data.sqlNumeric.sign = static_cast< SQLCHAR >( bSign ? 1 : 2 );
		memcpy_s( m_data.sqlNumeric.val, SQL_MAX_NUMERIC_LEN, contents.Data( ), nDigits );
	}

	SetData( SQL_PARAM_INPUT, SQL_C_NUMERIC, SQL_NUMERIC, m_data.sqlNumeric.precision, static_cast< SQLSMALLINT >( nDigits ), &m_data.sqlNumeric, sizeof( SQL_NUMERIC_STRUCT ), sizeof( SQL_NUMERIC_STRUCT ) );

	return true;
}

bool CBindParam::SetOutputParameter( Isolate* isolate, Local< Object > value )
{
	HandleScope scope( isolate );
	const auto context = isolate->GetCurrentContext( );

	auto paramType = value->Get( context, Nan::New( "paramType" ).ToLocalChecked( ) ).ToLocalChecked( );
	auto ref = value->Get( context, Nan::New( "reference" ).ToLocalChecked( ) ).ToLocalChecked( );

	auto length = value->Get( context, Nan::New( "length" ).ToLocalChecked( ) ).ToLocalChecked( );
	auto precision = value->Get( context, Nan::New( "precision" ).ToLocalChecked( ) ).ToLocalChecked( );
	auto scale = value->Get( context, Nan::New( "scale" ).ToLocalChecked( ) ).ToLocalChecked( );


	auto eType = static_cast< ESqlType >(  paramType->Uint32Value( context ).FromJust( ) );
	size_t nLength = static_cast< size_t >( length->IntegerValue( context ).FromJust( ) );
	uint32_t nPrecision = static_cast< uint32_t >( precision->Uint32Value( context ).FromJust( ) );
	uint32_t nScale = static_cast< uint32_t >( scale->Uint32Value( context ).FromJust( ) );


	switch( eType )
	{
		case ESqlType::eBit:
		{
			SetPrimitve< SQLCHAR >( SQL_PARAM_OUTPUT, SQL_C_BIT, SQL_BIT, &m_data.bValue );
			break;
		}
		case ESqlType::eTinyint:
		{
			SetPrimitve< int32_t >( SQL_PARAM_OUTPUT, SQL_C_SLONG, SQL_TINYINT, &m_data.nInt32 );
			break;
		}
		case ESqlType::eSmallint:
		{
			SetPrimitve< int32_t >( SQL_PARAM_OUTPUT, SQL_C_SLONG, SQL_SMALLINT, &m_data.nInt32 );
			break;
		}
		case ESqlType::eInt32:
		{
			SetPrimitve< int32_t >( SQL_PARAM_OUTPUT, SQL_C_SLONG, SQL_INTEGER, &m_data.nInt32 );
			break;
		}
		case ESqlType::eUint32: 
		{
			SetPrimitve< uint32_t >( SQL_PARAM_OUTPUT, SQL_C_ULONG, SQL_BIGINT, &m_data.nUint32 );
			break;
		}
		case ESqlType::eBigInt:
		{
			SetPrimitve< int64_t >( SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, &m_data.nInt64 );
			break;
		}
		case ESqlType::eReal:
		{
			SetPrimitve< double >( SQL_PARAM_OUTPUT, SQL_C_DOUBLE, SQL_DOUBLE, &m_data.dNumber );
			break;
		}
		case ESqlType::eChar:
		{
			m_data.stringDesc.Alloc( EStringType::eAnsi, nLength );
			SetData( SQL_PARAM_OUTPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, m_data.stringDesc.m_stringData.pString, nLength, nLength );

			break;
		}
		case ESqlType::eNChar:
		{
			m_data.stringDesc.Alloc( EStringType::eUnicode, nLength );
			//size*2
			SetData( SQL_PARAM_OUTPUT, SQL_C_WCHAR, SQL_WCHAR, 0, 0, m_data.stringDesc.m_stringData.pWString, nLength, nLength );

			break;
		}
		case ESqlType::eVarChar:
		{
			m_data.stringDesc.Alloc( EStringType::eAnsi, nLength );
			SetData( SQL_PARAM_OUTPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, m_data.stringDesc.m_stringData.pString, nLength, nLength );
			break;
		}
		case ESqlType::eNVarChar:
		{
			m_data.stringDesc.Alloc( EStringType::eUnicode, nLength );
			SetData( SQL_PARAM_OUTPUT, SQL_C_WCHAR, SQL_WVARCHAR, 0, 0, m_data.stringDesc.m_stringData.pWString, nLength, nLength );

			break;
		}
		case ESqlType::eBinary:
			break;
		case ESqlType::eVarBinary:
			break;
		case ESqlType::eDate:
			break;
		case ESqlType::eTimestamp:
			break;
		case ESqlType::eNumeric:
			break;
		case ESqlType::eLongVarChar:
		case ESqlType::eLongNVarChar:
		case ESqlType::eLongVarBinary:
		default:
		{
			return false;
		}
	}

	m_paramRef.Reset( isolate, ref );


	return true;
}
