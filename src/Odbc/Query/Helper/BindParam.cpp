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

	SetData( SQL_C_BINARY, SQL_LONGVARBINARY, nLength, 0, ( SQLPOINTER )this, 0, SQL_DATA_AT_EXEC );

	return true;
}

bool CBindParam::SetNumeric( Isolate* isolate, Local< Object > value )
{
	HandleScope scope( isolate );
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

	auto nPrecision = precision.As< Uint32 >( )->Uint32Value( context ).FromJust( );
	auto nScale = scale.As< Uint32 >( )->Uint32Value( context ).FromJust( );
	auto bSign = sign.As< Boolean >( )->BooleanValue( context ).FromJust( );
	auto contents = buffer.As< Uint8Array >( )->Buffer( )->GetContents( );

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

bool CBindParam::SetDate( Isolate* isolate, Local< Object > value )
{
	HandleScope scope( isolate );
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

	auto ms = date.As< Date >( )->IntegerValue( context ).FromJust( );
	{
		CJSDate::ToSqlDate( ms, m_data.sqlDate );
	}

	SetData( SQL_C_TYPE_DATE, SQL_TYPE_DATE, 0, 0, &m_data.sqlDate, sizeof( SQL_DATE_STRUCT ), sizeof( SQL_DATE_STRUCT ) );
	return true;
}

bool CBindParam::SetOutputParameter( Isolate* isolate, Local< Object > value )
{
	HandleScope scope( isolate );
	const auto context = isolate->GetCurrentContext( );

	auto _paramType = value->Get( context, Nan::New( "paramType" ).ToLocalChecked( ) );
	if( _paramType.IsEmpty( ) )
	{
		return false;
	}

	auto paramType = _paramType.ToLocalChecked( );
	if( !paramType->IsUint32( ) )
	{
		return false;
	}

	uint32_t nParamType = paramType->Uint32Value( context ).FromJust( );
	if( nParamType >= eSqlOutputType::eMax )
	{
		return false;
	}

	switch( static_cast< eSqlOutputType >( nParamType ) )
	{
		case eBitOutput:
		{
			SetPrimitve< SQLCHAR >( SQL_C_BIT, SQL_BIT, &m_data.bValue );
			m_nInputOutputType = SQL_PARAM_OUTPUT;
			break;
		}
		case eTinyintOutput:
		{
			SetPrimitve< int32_t >( SQL_C_LONG, SQL_TINYINT, &m_data.bValue );
			m_nInputOutputType = SQL_PARAM_OUTPUT;

			break;
		}

		case eSmallint: 
		{
			SetPrimitve< int32_t >( SQL_C_LONG, SQL_SMALLINT, &m_data.bValue );
			m_nInputOutputType = SQL_PARAM_OUTPUT;

			break;
		}


		case eInt: 
		{
			SetPrimitve< int32_t >( SQL_C_LONG, SQL_INTEGER, &m_data.bValue );
			m_nInputOutputType = SQL_PARAM_OUTPUT;

			break;
		}

		case eBigInt:
		{
			SetPrimitve< int64_t >( SQL_C_SBIGINT, SQL_BIGINT, &m_data.nInt64 );
			m_nInputOutputType = SQL_PARAM_OUTPUT;

			break;
		}

		case eFloat: 
		{
			break;
		}

		case eReal:
			break;

		case eChar:
			break;

		case eNChar:
			break;

		case eVarChar:
			break;

		case eNVarChar:
			break;

		case eBinary:
			break;

		case eVarBinary:
			break;

		case eDate:
			break;

		case eTimestamp:
			break;

		case eNumeric:
			break;

		case eMax:
		default:
		{
			assert( false );
			break;
		}
	}


	auto _ref = value->Get( context, Nan::New( "ref" ).ToLocalChecked( ) );
	if( _ref.IsEmpty( ) )
	{
		return false;
	}

	m_paramRef.Reset( isolate, _paramType.ToLocalChecked( ) );

	return true;
}
