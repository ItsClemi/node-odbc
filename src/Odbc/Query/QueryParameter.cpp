// File: QueryParameter.cpp
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
#include "QueryParameter.h"

using namespace v8;

#ifdef _WINDOWS
#pragma push_macro("min")
#pragma push_macro( "max" )

#undef min
#undef max
#endif

CQueryParameter::CQueryParameter( )
{

}

CQueryParameter::~CQueryParameter( )
{
	for( auto& i : m_vecParameter )
	{
		i.Dispose( );
	}
	m_vecParameter.clear( );
}

bool CQueryParameter::AddParameter( Isolate* isolate, Local< Value > value, CBindParam* pParam )
{
	HandleScope scope( isolate );
	const auto context = isolate->GetCurrentContext( );


	if( value->IsNull( ) )
	{
		pParam->SetNull( );
	}
	else if( value->IsBoolean( ) )
	{
		pParam->SetBool( value->BooleanValue( context ).FromJust( ) );
	}
	else if( value->IsInt32( ) )
	{
		pParam->SetInt32( value->Int32Value( context ).FromJust( ) );
	}
	else if( value->IsUint32( ) )
	{
		pParam->SetUint32( value->Uint32Value( context ).FromJust( ) );
	}
	else if( value->IsNumber( ) )
	{
		double d = value->NumberValue( context ).FromJust( );

		if( std::isnan( d ) || !std::isfinite( d ) )
		{
			return false;
		}
		else if( d == floor( d ) &&
			d >= std::numeric_limits< int64_t >::min( ) &&
			d <= std::numeric_limits< int64_t >::max( )
			)
		{
			pParam->SetInt64( value->IntegerValue( context ).FromJust( ) );
		}
		else
		{
			pParam->SetDouble( value->NumberValue( context ).FromJust( ) );
		}
	}
	else if( value->IsString( ) )
	{
		pParam->SetString( value.As< String >( ) );
	}
	else if( value->IsDate( ) )
	{
		pParam->SetTimestamp( static_cast< int64_t >( value->NumberValue( context ).FromJust( ) ) );
	}
	//else if( node::Buffer::HasInstance( value ) )
	//{
	//	pParam->SetBuffer( value );
	//}
	else if( IsComplexType( isolate, value, ID_INPUT_STREAM ) )
	{
		if( !pParam->SetStream( isolate, value.As< Object >() ) )
		{
			return false;
		}
	}
	else if( IsComplexType( isolate, value, ID_NUMERIC_VALUE ) )
	{
		if( !pParam->SetNumeric( isolate, value.As< Object >( ) ) )
		{
			return false;
		}
	}
	else if( IsComplexType( isolate, value, ID_DATE_VALUE ) )
	{
		if( !pParam->SetDate( isolate, value.As< Object >( ) ) )
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}

#ifdef _WINDOWS

#pragma pop_macro( "min" )
#pragma pop_macro( "max" )

#endif