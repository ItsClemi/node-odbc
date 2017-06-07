// File: EConnection.cpp
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
#include "EConnection.h"
#include "EModuleHelper.h"
#include "EPreparedQuery.h"

#include "Odbc/Connection/ConnectionPool.h"
#include "Odbc/Query/Query.h"


using namespace v8;


Nan::Persistent< Function >		EConnection::constructor;


NAN_MODULE_INIT( EConnection::InitializeModule )
{
	const auto strClassName = Nan::New( "Connection" ).ToLocalChecked( );


	const auto tpl = Nan::New< FunctionTemplate >( EConnection::New );
	tpl->InstanceTemplate( )->SetInternalFieldCount( 1 );

	tpl->SetClassName( strClassName );
	{
		Nan::SetPrototypeMethod( tpl, "connect", EConnection::Connect );
		Nan::SetPrototypeMethod( tpl, "disconnect", EConnection::Disconnect );

		//Nan::SetPrototypeMethod( tpl, "setResilienceStrategy", EConnection::SetResilienceStrategy );
		Nan::SetPrototypeMethod( tpl, "prepareQuery", EConnection::PrepareQuery );
		Nan::SetPrototypeMethod( tpl, "executeQuery", EConnection::ExecuteQuery );
		Nan::SetPrototypeMethod( tpl, "getInfo", EConnection::GetInfo );
	}

	constructor.Reset( Nan::GetFunction( tpl ).ToLocalChecked( ) );
	Nan::Set( target, strClassName, Nan::GetFunction( tpl ).ToLocalChecked( ) );
}


NAN_METHOD( EConnection::New )
{
	auto isolate = info.GetIsolate( );
	HandleScope scope( isolate );
	
	const auto context = isolate->GetCurrentContext( );

	if( info.IsConstructCall( ) )
	{
		V8_TYPE_VALIDATE( info[ 0 ]->IsUndefined( ) || info[ 0 ]->IsObject( ), "^advancedProps: is not undefined or an object" );

		const auto pThis = new EConnection( );
		{
			pThis->Wrap( info.This( ) );
			pThis->GetPool( )->SetSharedInstance( pThis->m_pPool );
		}

		if( info[ 0 ]->IsObject( ) )
		{
			if( !pThis->GetPool( )->ReadConnectionProps( info.GetIsolate( ), info[ 0 ].As< Object >( ) ) )
			{
				return;
			}
		}

		info.GetReturnValue( ).Set( info.This( ) );
	}
	else
	{
		const int argc = 1;
		Local< Value > argv[ argc ] = { info[ 0 ] };

		const auto cons = Nan::New( constructor );
		info.GetReturnValue( ).Set( cons->NewInstance( context, argc, argv ).ToLocalChecked( ) );
	}
}

NAN_METHOD( EConnection::Connect )
{
	HandleScope scope( info.GetIsolate( ) );

	V8_TYPE_VALIDATE( info[ 0 ]->IsString( ), "^connectionString: is not a string" );
	V8_TYPE_VALIDATE( info[ 1 ]->IsUndefined( ) || info[ 1 ]->Uint32Value( ), "^connectionTimeout: is not undefined or int32" );

	const auto pThis = Nan::ObjectWrap::Unwrap< EConnection >( info.This( ) );
	V8_RUNTIME_VALIDATE( pThis->GetPool( )->GetState( ) == EPoolState::eNone, "invalid pool state" );

	const auto szConnectionString = FromV8String( info[ 0 ].As< String >( ) );

	uint32_t nTimeout = 0;
	if( info[ 0 ]->IsUint32( ) )
	{
		nTimeout = info[ 0 ]->Uint32Value( Nan::GetCurrentContext( ) ).FromJust( );
	}

	if( !pThis->GetPool( )->InitializePool( szConnectionString, nTimeout ) )
	{
		return;
	}

	info.GetReturnValue( ).Set( info.This( ) );
}

NAN_METHOD( EConnection::Disconnect )
{
	auto isolate = info.GetIsolate( );
	HandleScope scope( isolate );

	V8_RUNTIME_VALIDATE( info[ 0 ]->IsFunction( ), "cb: not a function" );

	const auto pThis = Nan::ObjectWrap::Unwrap< EConnection >( info.This( ) );
	V8_RUNTIME_VALIDATE( pThis->GetPool( )->IsReady( ), "invalid pool state (connected to your datasource?)" );

	pThis->GetPool( )->RequestDisconnect( isolate, info[ 0 ].As< Function >( ) );
}

NAN_METHOD( EConnection::SetResilienceStrategy )
{
	auto isolate = info.GetIsolate( );
	HandleScope scope( isolate );

	V8_RUNTIME_VALIDATE( info[ 0 ]->IsObject( ), "strategy: not an object" );

	const auto pThis = Nan::ObjectWrap::Unwrap< EConnection >( info.This( ) );
	V8_RUNTIME_VALIDATE( pThis->GetPool( )->GetState( ) == EPoolState::eNone, "invalid pool state (connected to your datasource?)" );

	if( !pThis->GetPool( )->ReadResilienceStrategy( isolate, info[ 0 ].As< Object >( ) ) )
	{
		return;
	}

	info.GetReturnValue( ).Set( info.This( ) );
}

NAN_METHOD( EConnection::PrepareQuery )
{
	auto isolate = info.GetIsolate( );
	HandleScope scope( isolate );

	V8_RUNTIME_VALIDATE( info[ 0 ]->IsString( ), "query: invalid parameter type" );

	const auto pThis = Nan::ObjectWrap::Unwrap< EConnection >( info.This( ) );
	V8_RUNTIME_VALIDATE( pThis->GetPool( )->IsReady( ), "invalid pool state (connected to your datasource?)" );


	const auto pQuery = pThis->GetPool( )->CreateQuery( );

	pQuery->InitializeQuery( FromV8String( info[ 0 ].As< String >( ) ) );
	{
		if( !pQuery->GetQueryParam( )->BindParameters( isolate, info, 1 ) )
		{
			return;
		}
	}

	info.GetReturnValue( ).Set(
		EPreparedQuery::CreateInstance( isolate, pQuery )
	);
}

NAN_METHOD( EConnection::ExecuteQuery )
{
	auto isolate = info.GetIsolate( );

	HandleScope scope( isolate );
	const auto context = isolate->GetCurrentContext( );

	V8_TYPE_VALIDATE( info[ 0 ]->IsFunction( ) || info[ 0 ]->IsUint32( ), "cb || eFetchMode: is not a callback or a number" );


	const auto pThis = Nan::ObjectWrap::Unwrap< EConnection >( info.This( ) );
	V8_RUNTIME_VALIDATE( pThis->GetPool( )->IsReady( ), "invalid pool state (connected to your datasource?)" );


	const auto pQuery = pThis->GetPool( )->CreateQuery( );

	if( info[ 0 ]->IsFunction( ) )
	{
		V8_TYPE_VALIDATE( info[ 1 ]->IsString( ), "query: is not a string" );

		if( !pQuery->SetParameters(
			isolate,
			EFetchMode::eSingle,
			info[ 0 ].As< Function >( ),
			FromV8String( info[ 1 ].As< String >( ) ),
			info,
			2
		) )
		{
			return;
		}
	}
	else
	{
		uint32_t nFetchMode = info[ 0 ]->Uint32Value( context ).FromJust( );
		V8_TYPE_VALIDATE( nFetchMode < ToUnderlyingType( EFetchMode::eMax ), "eMode: invalid number" );

		V8_TYPE_VALIDATE( info[ 1 ]->IsFunction( ), "cb: is not a function" );
		V8_TYPE_VALIDATE( info[ 2 ]->IsString( ), "query: is not a string" );


		if( !pQuery->SetParameters(
			isolate,
			static_cast< EFetchMode >( nFetchMode ),
			info[ 1 ].As< Function >( ),
			FromV8String( info[ 2 ].As< String >( ) ),
			info,
			3
		) )
		{
			return;
		}
	}
		

	pThis->GetPool( )->ExecuteQuery( pQuery );

	isolate->AdjustAmountOfExternalAllocatedMemory( sizeof( CQuery ) );
}

NAN_METHOD( EConnection::GetInfo )
{
	HandleScope scope( info.GetIsolate( ) );

	const auto pThis = Nan::ObjectWrap::Unwrap< EConnection >( info.This( ) );

	V8_RUNTIME_VALIDATE( pThis->GetPool( )->IsReady( ), "invalid pool state (connected to your datasource?)" );

	info.GetReturnValue( ).Set(
		pThis->m_pPool->GetConnectionInfo( info.GetIsolate( ) )
	);
}


#ifdef _DEBUG
extern std::atomic< size_t >	g_nJsConnectionCount;
#endif

EConnection::EConnection( )
{
#ifdef _DEBUG
	g_nJsConnectionCount++;
#endif
}

EConnection::~EConnection( )
{
#ifdef _DEBUG
	g_nJsConnectionCount--;
#endif
}