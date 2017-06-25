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
	friend class CQuery;
	friend class CResultSet;

public:
	CQueryParameter( );
	virtual ~CQueryParameter( );

public:
	bool AddParameters( v8::Isolate* isolate, const v8::Local< v8::Array > args )
	{
		v8::HandleScope scope( isolate );
		const auto context = isolate->GetCurrentContext( );

		if( args->Length( ) <= 0 )
		{
			return true;
		}

		const size_t nParams = static_cast< size_t >( args->Length( ) / 2 );

		m_vecParameter.resize( nParams );


		for( size_t i = 0; i < nParams; i++ )
		{
			const ESqlType eType = static_cast< ESqlType >(
				args->Get( context, static_cast< int >( i * 2 ) ).ToLocalChecked( )->Uint32Value( context ).FromJust( ) 
				);

			const auto value = args->Get( context, static_cast< int >( ( i * 2 ) + 1  ) ).ToLocalChecked( );


			if( !AddParameter( isolate, eType, value, &m_vecParameter[ i ] ) )
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
	bool HasOutputParams( )
	{
		return m_bOutputParameters;
	}

private:
	bool	m_bOutputParameters = false;

	std::vector< CBindParam, tbb::scalable_allocator< CBindParam > >	m_vecParameter;
};