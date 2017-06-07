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
#include "EModuleHelper.h"

#include "Odbc/Connection/ConnectionPool.h"
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
		Nan::SetPrototypeMethod( tpl, "enableReturnValue", EPreparedQuery::EnableReturnValue );
		Nan::SetPrototypeMethod( tpl, "enableMetaData", EPreparedQuery::EnableMetaData );
		Nan::SetPrototypeMethod( tpl, "enableSlowQuery", EPreparedQuery::EnableSlowQuery );
		Nan::SetPrototypeMethod( tpl, "enableTransaction", EPreparedQuery::EnableTransaction );

		Nan::SetPrototypeMethod( tpl, "setQueryTimeout", EPreparedQuery::SetQueryTimeout );

		Nan::SetPrototypeMethod( tpl, "toSingle", EPreparedQuery::ToSingle );
		Nan::SetPrototypeMethod( tpl, "toArray", EPreparedQuery::ToArray );

		Nan::SetPrototypeMethod( tpl, "rollback", EPreparedQuery::Rollback );
		Nan::SetPrototypeMethod( tpl, "commit", EPreparedQuery::Commit );

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


NAN_METHOD( EPreparedQuery::EnableReturnValue )
{
	HandleScope scope( info.GetIsolate( ) );

	const auto pThis = Nan::ObjectWrap::Unwrap< EPreparedQuery >( info.This( ) );
	V8_RUNTIME_VALIDATE( pThis->GetQuery( )->IsIdle( ), "invalid query state: query is running" );

	pThis->GetQuery( )->EnableReturnValue( );

	info.GetReturnValue( ).Set( info.This( ) );
}

NAN_METHOD( EPreparedQuery::EnableMetaData )
{
	HandleScope scope( info.GetIsolate( ) );

	const auto pThis = Nan::ObjectWrap::Unwrap< EPreparedQuery >( info.This( ) );
	V8_RUNTIME_VALIDATE( pThis->GetQuery( )->IsIdle( ), "invalid query state: query is running" );

	pThis->GetQuery( )->GetResultSet( )->EnableMetaData( );

	info.GetReturnValue( ).Set( info.This( ) );
}

NAN_METHOD( EPreparedQuery::EnableSlowQuery )
{
	HandleScope scope( info.GetIsolate( ) );

	const auto pThis = Nan::ObjectWrap::Unwrap< EPreparedQuery >( info.This( ) );
	V8_RUNTIME_VALIDATE( pThis->GetQuery( )->IsIdle( ), "invalid query state: query is running" );


	pThis->GetQuery( )->EnableSlowQuery( );

	info.GetReturnValue( ).Set( info.This( ) );
}

NAN_METHOD( EPreparedQuery::EnableTransaction )
{
	HandleScope scope( info.GetIsolate( ) );

	const auto pThis = Nan::ObjectWrap::Unwrap< EPreparedQuery >( info.This( ) );
	V8_RUNTIME_VALIDATE( pThis->GetQuery( )->IsIdle( ), "invalid query state: query is running" );

	pThis->GetQuery( )->EnableTransaction( info.GetIsolate( ), info.This( ) );

	info.GetReturnValue( ).Set( info.This( ) );
}


NAN_METHOD( EPreparedQuery::SetQueryTimeout )
{
	auto isolate = info.GetIsolate( );
	HandleScope scope( isolate );
	const auto context = isolate->GetCurrentContext( );

	V8_TYPE_VALIDATE( info[ 0 ]->IsUint32( ), "setQueryTimeout: is not an uint32" );
	
	const auto pThis = Nan::ObjectWrap::Unwrap< EPreparedQuery >( info.This( ) );
	V8_RUNTIME_VALIDATE( pThis->GetQuery( )->IsIdle( ), "invalid query state: query is running" );

	auto nTimeout = info[ 0 ].As< Uint32 >( )->Uint32Value( context ).FromJust( );

	pThis->GetQuery( )->SetQueryTimeout( nTimeout );

	info.GetReturnValue( ).Set( info.This( ) );
}


NAN_METHOD( EPreparedQuery::ToSingle )
{
	auto isolate = info.GetIsolate( );
	HandleScope scope( isolate );

	V8_TYPE_VALIDATE( info[ 0 ]->IsUndefined( ) || info[ 0 ]->IsFunction( ), "not undefined or function");

	const auto pThis = Nan::ObjectWrap::Unwrap< EPreparedQuery >( info.This( ) );
	V8_RUNTIME_VALIDATE( pThis->GetQuery( )->IsActive( ), "invalid query state: query is running" );
	
	auto pQuery = pThis->GetQuery( );

	pQuery->GetResultSet()->SetFetchMode( EFetchMode::eSingle );


	if( info[ 0 ]->IsFunction( ) )
	{
		pQuery->GetResultSet( )->SetCallback( isolate, info[ 0 ].As< Function >( ) );
	}
	else
	{
		//pQuery->GetResultSet( )->SetPromise( );
	}

	pQuery->GetPool( )->ExecuteQuery( pQuery );


	return info.GetReturnValue( ).Set( 
		EModuleHelper::CreatePromise( isolate, info.This( ) )
	);
}

NAN_METHOD( EPreparedQuery::ToArray )
{
	auto isolate = info.GetIsolate( );
	HandleScope scope( isolate );

	V8_TYPE_VALIDATE( info[ 0 ]->IsUndefined( ) || info[ 0 ]->IsFunction( ), "not undefined or function" );

	const auto pThis = Nan::ObjectWrap::Unwrap< EPreparedQuery >( info.This( ) );
	V8_RUNTIME_VALIDATE( pThis->GetQuery( )->IsActive( ), "invalid query state: query is running" );

	auto pQuery = pThis->GetQuery( );

	pQuery->GetResultSet( )->SetFetchMode( EFetchMode::eArray );

	if( info[ 0 ]->IsFunction( ) )
	{
		pQuery->GetResultSet( )->SetCallback( isolate, info[ 0 ].As< Function >( ) );
	}
	else
	{
		//pQuery->GetResultSet( )->SetPromise( );
	}

	pQuery->GetPool( )->ExecuteQuery( pQuery );

	return info.GetReturnValue( ).Set(
		EModuleHelper::CreatePromise( isolate, info.This( ) )
	);
}


NAN_METHOD( EPreparedQuery::Rollback )
{
	HandleScope scope( info.GetIsolate( ) );

	const auto pThis = Nan::ObjectWrap::Unwrap< EPreparedQuery >( info.This( ) );
	V8_RUNTIME_VALIDATE( pThis->GetQuery( )->IsActive( ), "invalid query state: query is not running" );


}

NAN_METHOD( EPreparedQuery::Commit )
{
	HandleScope scope( info.GetIsolate( ) );

	const auto pThis = Nan::ObjectWrap::Unwrap< EPreparedQuery >( info.This( ) );
	V8_RUNTIME_VALIDATE( pThis->GetQuery( )->IsActive( ), "invalid query state: query is not running" );

	
}


NAN_METHOD( EPreparedQuery::SetPromiseInfo )
{
	Isolate* isolate = info.GetIsolate( );
	HandleScope scope( isolate );

	V8_RUNTIME_VALIDATE( info[ 0 ]->IsFunction( ), "resolve: invalid type" );
	V8_RUNTIME_VALIDATE( info[ 1 ]->IsFunction( ), "reject: invalid type" );

	const auto pThis = Unwrap< EPreparedQuery >( info.This( ) );
	{
		pThis->GetQuery( )->GetResultSet( )->SetPromise( isolate, info[ 0 ].As< Function >( ), info[ 1 ].As< Function >( ) );
	}
}


EPreparedQuery::EPreparedQuery( )
{ }

EPreparedQuery::~EPreparedQuery( )
{ 
}
