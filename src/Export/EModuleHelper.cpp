// File: EModuleHelper.cpp
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
#include "EModuleHelper.h"

using namespace v8;


Persistent< Function >	EModuleHelper::s_fnWriteStreamInitializer;
Persistent< Function >	EModuleHelper::s_fnReadStreamInitializer;
Persistent< Function >	EModuleHelper::s_fnPromiseInitializer;



void EModuleHelper::InitializeModule( const Local< Object > exports )
{
	NODE_SET_METHOD( exports, "setWriteStreamInitializer", EModuleHelper::SetWriteStreamInitializer );
	NODE_SET_METHOD( exports, "setReadStreamInitializer", EModuleHelper::SetReadStreamInitializer );
	NODE_SET_METHOD( exports, "setPromiseInitializer", EModuleHelper::SetPromiseInitializer );

	NODE_SET_METHOD( exports, "processNextChunk", EModuleHelper::ProcessNextChunk );
	NODE_SET_METHOD( exports, "requestNextChunk", EModuleHelper::RequestNextChunk );
}

void EModuleHelper::SetWriteStreamInitializer( const FunctionCallbackInfo< Value >& args )
{
	const auto isolate = args.GetIsolate( );
	HandleScope scope( isolate );

	V8_RUNTIME_VALIDATE( args[ 0 ]->IsFunction( ), "cb: is not a function" );

	s_fnWriteStreamInitializer.Reset( isolate, args[ 0 ].As< Function >( ) );
}

void EModuleHelper::SetReadStreamInitializer( const FunctionCallbackInfo< Value >& args )
{
	const auto isolate = args.GetIsolate( );
	HandleScope scope( isolate );

	V8_RUNTIME_VALIDATE( args[ 0 ]->IsFunction( ), "cb: is not a function" );

	s_fnReadStreamInitializer.Reset( isolate, args[ 0 ].As< Function >( ) );
}

void EModuleHelper::SetPromiseInitializer( const FunctionCallbackInfo< Value >& args )
{
	const auto isolate = args.GetIsolate( );
	HandleScope scope( isolate );

	V8_RUNTIME_VALIDATE( args[ 0 ]->IsFunction( ), "cb: is not a function" );


	s_fnPromiseInitializer.Reset( isolate, args[ 0 ].As< Function >( ) );
}


void EModuleHelper::ProcessNextChunk( const FunctionCallbackInfo< Value >& args )
{
	const auto isolate = args.GetIsolate( );
	HandleScope scope( isolate );


}

void EModuleHelper::RequestNextChunk( const FunctionCallbackInfo< Value >& args )
{
	const auto isolate = args.GetIsolate( );
	HandleScope scope( isolate );


}

const Local< Value > EModuleHelper::CreatePromise( Isolate* isolate, const Local< Value > _this )
{
	EscapableHandleScope scope( isolate );
	const auto fnInitializer = node::PersistentToLocal( isolate, EModuleHelper::s_fnPromiseInitializer );
	
	const unsigned argc = 1;
	Local< Value > argv[ argc ] = { _this };

	TryCatch try_catch;

	auto promise = node::MakeCallback( isolate, Object::New( isolate ), fnInitializer, argc, argv );

	if( try_catch.HasCaught( ) )
	{
		try_catch.ReThrow( );
		return scope.Escape( Undefined( isolate ) );
	}

	return scope.Escape( promise );
}


// Local<FunctionTemplate> function_template = FunctionTemplate::New( isolate );
// // Instances of this function have room for 1 internal field
// function_template->InstanceTemplate( )->SetInternalFieldCount( 1 );
// 
// Local<Object> object = function_template->GetFunction( )->NewInstance( );
// static_assert( sizeof( void* ) == sizeof( uint64_t ) );
// uint64_t integer = 1;
// object->SetAlignedPointerInInternalField( 0, reinterpret_cast< void* >( integer ) );
// uint64_t result = reinterpret_cast< uint64_t >( object->GetAlignedPointerInInternalField( 0 ) );
