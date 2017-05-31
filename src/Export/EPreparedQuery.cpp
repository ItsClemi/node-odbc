// File: EPreparedQuery.cpp
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
#include "EPreparedQuery.h"

#include "Odbc/Query/Query.h"

using namespace v8;


Nan::Persistent< Function >	EPreparedQuery::constructor;


NAN_MODULE_INIT( EPreparedQuery::InitializeModule )
{
	const auto strClassName = Nan::New( "ISqlQuery" ).ToLocalChecked( );

	const auto tpl = Nan::New< FunctionTemplate >( EPreparedQuery::New );

	tpl->InstanceTemplate( )->SetInternalFieldCount( 1 );

	tpl->SetClassName( strClassName );
	{
		Nan::SetPrototypeMethod( tpl, "setPromiseInfo", EPreparedQuery::SetPromiseInfo );
	}

	constructor.Reset( Nan::GetFunction( tpl ).ToLocalChecked( ) );
	Nan::Set( target, strClassName, Nan::GetFunction( tpl ).ToLocalChecked( ) );
}


Local< Object > EPreparedQuery::CreateInstance( Isolate* isolate, const std::shared_ptr< CQuery > pQuery )
{
	EscapableHandleScope scope( isolate );
	const auto context = isolate->GetCurrentContext( );

	const auto fnCtor = node::PersistentToLocal( isolate, constructor );

	const auto query = fnCtor->NewInstance( context ).ToLocalChecked( );
	{
		Unwrap< EPreparedQuery >( query )->SetQuery( pQuery );
	}

	return scope.Escape( query );
}


NAN_METHOD( EPreparedQuery::New )
{
	const auto pQuery = new EPreparedQuery( );
	{
		pQuery->Wrap( info.This( ) );
	}
	info.GetReturnValue( ).Set( info.This( ) );
}


NAN_METHOD( EPreparedQuery::AddParameter )
{
	auto isolate = info.GetIsolate( );
	HandleScope scope( isolate );

	//const auto pThis = Nan::ObjectWrap::Unwrap< EPreparedQuery >( info.This( ) );



}

NAN_METHOD( EPreparedQuery::AddResultSetHandler )
{

}

NAN_METHOD( EPreparedQuery::EnableReturnValue )
{
	HandleScope scope( info.GetIsolate( ) );

	const auto pThis = Nan::ObjectWrap::Unwrap< EPreparedQuery >( info.This( ) );

	pThis->GetQuery( )->EnableReturnValue( );

	info.GetReturnValue( ).Set( info.This( ) );
}

NAN_METHOD( EPreparedQuery::EnableMetaData )
{
	HandleScope scope( info.GetIsolate( ) );

	const auto pThis = Nan::ObjectWrap::Unwrap< EPreparedQuery >( info.This( ) );

	pThis->GetQuery( )->EnableMetaData( );

	info.GetReturnValue( ).Set( info.This( ) );
}

NAN_METHOD( EPreparedQuery::EnableSlowQuery )
{
	HandleScope scope( info.GetIsolate( ) );

	const auto pThis = Nan::ObjectWrap::Unwrap< EPreparedQuery >( info.This( ) );

	pThis->GetQuery( )->EnableSlowQuery( );

	info.GetReturnValue( ).Set( info.This( ) );
}

NAN_METHOD( EPreparedQuery::SetQueryTimeout )
{
	auto isolate = info.GetIsolate( );
	HandleScope scope( isolate );
	const auto context = isolate->GetCurrentContext( );

	V8_TYPE_VALIDATE( info[ 0 ]->IsUint32( ), "setQueryTimeout: not an uint32" );
	V8_RUNTIME_VALIDATE( info.Length( ) > 1, "too many arguments" );

	const auto pThis = Nan::ObjectWrap::Unwrap< EPreparedQuery >( info.This( ) );

	auto nTimeout = info[ 0 ].As< Uint32 >( )->Uint32Value( context ).FromJust( );

	pThis->GetQuery( )->SetQueryTimeout( nTimeout );

	info.GetReturnValue( ).Set( info.This( ) );
}

NAN_METHOD( EPreparedQuery::SetChunkSize )
{
	auto isolate = info.GetIsolate( );
	HandleScope scope( isolate );
	const auto context = isolate->GetCurrentContext( );
	
	V8_TYPE_VALIDATE( info[ 0 ]->IsUint32( ), "chunkSize: not an uint32" );
	V8_RUNTIME_VALIDATE( info.Length( ) > 1, "too many arguments" );

	const auto pThis = Nan::ObjectWrap::Unwrap< EPreparedQuery >( info.This( ) );

	auto nChunkSize = info[ 0 ].As< Uint32 >( )->Uint32Value( context ).FromJust( );
	V8_RUNTIME_VALIDATE( nChunkSize >= 1, "chunkSize is too small" );

	pThis->GetQuery( )->SetChunkSize( nChunkSize );

	info.GetReturnValue( ).Set( info.This( ) );
}

NAN_METHOD( EPreparedQuery::ToSingle )
{

}

NAN_METHOD( EPreparedQuery::ToArray )
{

}

NAN_METHOD( EPreparedQuery::Rollback )
{

}

NAN_METHOD( EPreparedQuery::Commit )
{

}

NAN_METHOD( EPreparedQuery::AsTransaction )
{

}

NAN_METHOD( EPreparedQuery::Execute )
{

}

NAN_METHOD( EPreparedQuery::SetPromiseInfo )
{
	Isolate* isolate = info.GetIsolate( );
	HandleScope scope( isolate );

	V8_RUNTIME_VALIDATE( info[ 0 ]->IsFunction( ), "resolve: invalid type" );
	V8_RUNTIME_VALIDATE( info[ 1 ]->IsFunction( ), "reject: invalid type" );

	const auto pThis = Unwrap< EPreparedQuery >( info.This( ) );
	{
		//RUNTIME_VALIDATE( pThis->GetQuery( )->HasPendingPromise( ) );

		//pThis->GetQuery( )->SetPromiseInfo( isolate, args[ 0 ].As< Function >( ), args[ 1 ].As< Function >( ) );
	}
}


EPreparedQuery::EPreparedQuery( )
{ }

EPreparedQuery::~EPreparedQuery( )
{ 
}
