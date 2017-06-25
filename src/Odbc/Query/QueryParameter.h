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
	static const uint32_t ID_DATE_VALUE = 2;
	static const uint32_t ID_OUTPUT_PARAMETER = 3;

public:
	CQueryParameter( );
	virtual ~CQueryParameter( );


public:
	bool BindParameters( v8::Isolate* isolate, const Nan::FunctionCallbackInfo<v8::Value>& args, const int nPos )
	{
		assert( ( nPos - 1 ) > 0 );

		v8::HandleScope scope( isolate );
		const auto context = isolate->GetCurrentContext( );

		if( args.Length( ) <= nPos )
		{
			return true;
		}

		const size_t nLength = static_cast< size_t >( args.Length( ) - nPos );
		m_vecParameter.resize( nLength );

		v8::Local< v8::Array > types = args[ nPos - 1 ];

		for( int i = 0; i < static_cast< int >( nLength ); i++ )
		{
			if( !AddParameter( 
				isolate, 
				static_cast< ESqlType >( types->Get( context, i ).ToLocalChecked( )->Uint32Value( context ).FromJust( ) ),
				args[ nPos + i ], 
				&m_vecParameter[ i ] 
			) )
			{
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
	bool AddParameter( v8::Isolate* isolate, ESqlType eType, v8::Local< v8::Value > value, CBindParam* pParam );

public:
	std::vector< CBindParam, tbb::scalable_allocator< CBindParam > >	m_vecParameter;
};