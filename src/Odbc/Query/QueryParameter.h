// File: QueryParameter.h
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

#include "Helper/BindParam.h"


class CQueryParameter
{
	static const uint32_t ID_INPUT_STREAM = 0;
	static const uint32_t ID_NUMERIC_VALUE = 1;
	static const uint32_t ID_TIMESTAMP_VALUE = 2;
	static const uint32_t ID_OUTPUT_PARAMETER = 3;


	friend class CQuery;
	friend class CResultSet;

public:
	CQueryParameter( );
	virtual ~CQueryParameter( );

public:
	void UpdateOutputParameters( v8::Isolate* isolate );


public:
	bool AddParameters( v8::Isolate* isolate, const Nan::FunctionCallbackInfo<v8::Value>& args, const int nOffset )
	{
		v8::HandleScope scope( isolate );
		const auto context = isolate->GetCurrentContext( );

		if( args.Length( ) <= nOffset )
		{
			return true;
		}


		const int nParams = static_cast< size_t >( args.Length( ) - nOffset );

		m_vecParameter.resize( static_cast< size_t >( nParams ) );


		for( int i = 0; i < nParams; i++ )
		{
			auto value = args[ nOffset + i ];

			if( !AddParameter( isolate, value, &m_vecParameter[ i ] ) )
			{
				auto _t = value->ToString( context ).ToLocalChecked( );
				auto ___t = FromV8String( _t );

				std::stringstream stream;
				{
					stream << "failed to bind query parameter at pos: " << i;
				}
				Nan::ThrowTypeError( Nan::New( stream.str( ) ).ToLocalChecked( ) );
				return false;
			}
		}

		return true;
	}


private:
	bool AddParameter( v8::Isolate* isolate, v8::Local< v8::Value > value, CBindParam* pParam );


private:
	bool IsComplexType( v8::Isolate* isolate, v8::Local< v8::Value > value, uint32_t id )
	{
		if( !value->IsObject( ) )
		{
			return false;
		}

		v8::HandleScope scope( isolate );
		const auto context = isolate->GetCurrentContext( );

		auto _typeId = value.As< v8::Object >( )->Get( context, Nan::New( "_typeId" ).ToLocalChecked( ) );
		if( _typeId.IsEmpty( ) )
		{
			return false;
		}

		auto typeId = _typeId.ToLocalChecked( );

		if( !typeId->IsUint32( ) )
		{
			return false;
		}

		return typeId->Uint32Value( context ).FromJust( ) == id;
	}


private:
	bool	m_bOutputParameters = false;

	std::vector< CBindParam/*, tbb::scalable_allocator< CBindParam >*/ >	m_vecParameter;
};