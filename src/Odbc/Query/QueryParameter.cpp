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


void CQueryParameter::UpdateOutputParameters( Isolate* isolate )
{
	if( !m_bOutputParameters )
	{
		return;
	}

	HandleScope scope( isolate );

	for( auto& i : m_vecParameter )
	{
		if( i.IsOutputParam( ) )
		{
			i.UpdateOutputParam( isolate );
		}
	}
}


bool CQueryParameter::AddParameter( Isolate* isolate, ESqlType eType, Local< Value > value, CBindParam* pParam )
{
	HandleScope scope( isolate );
	const auto context = isolate->GetCurrentContext( );

	switch( eType )
	{
		case ESqlType::eNull:
		{
			pParam->SetNull( );
			break;
		}
		case ESqlType::eBit:
		{
			pParam->SetBool( value->BooleanValue( context ).FromJust( ) );
			break;
		}
		case ESqlType::eInt32:
		{
			pParam->SetInt32( value->Int32Value( context ).FromJust( ) );
			break;
		}
		case ESqlType::eBigInt:
		{
			pParam->SetInt64( value->IntegerValue( context ).FromJust( ) );
			break;
		}
		case ESqlType::eReal:
		{
			pParam->SetDouble( value->NumberValue( context ).FromJust( ) );
			break;
		}
		case ESqlType::eNVarChar: 
		{
			pParam->SetString( value.As< String >( ) );
			break;
		}
		case ESqlType::eDate:
		{
			pParam->SetDate( static_cast< int64_t >( value->NumberValue( context ).FromJust( ) ) );
			break;
		}
		case ESqlType::eTimestamp: 
		{
			pParam->SetTimestamp( static_cast< int64_t >( value->NumberValue( context ).FromJust( ) ) );
			break;
		}
		case ESqlType::eNumeric: 
		{
			pParam->SetNumeric( isolate, value.As< Object >( ) );
			break;
		}
		case ESqlType::eSqlOutputVar:
		{
			m_bOutputParameters = true;

			pParam->SetOutputParameter( isolate, value.As< Object >( ) );
			break;
		}
		case ESqlType::eVarBinary:
		{
			pParam->SetBuffer( value );
			break;
		}

		case ESqlType::eLongNVarChar:
		{

			break;
		}
		case ESqlType::eLongVarBinary:
		{

			break;
		}

		default: 
		{
			return false;
		}
	}
	
	return true;
}
