// File: GlobalEnvironment.cpp
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
#include "GlobalEnvironment.h"

using namespace v8;

std::unique_ptr< SGlobalEnvironment > gEnv = std::make_unique< SGlobalEnvironment >( );

#ifdef _DEBUG
std::atomic< size_t >	g_nSqlHandleCount;
std::atomic< size_t >	g_nSqlConnectionCount;
std::atomic< size_t >	g_nSqlStatementCount;
std::atomic< size_t >	g_nConnectionPoolCount;
std::atomic< size_t >	g_nJsConnectionCount;
#endif



SGlobalEnvironment::SGlobalEnvironment( )
{

}

SGlobalEnvironment::~SGlobalEnvironment( )
{
	//-> dtor gets called first, if node crashes by exception
	DestroyGlobalEnvironment( );
}


bool InitializeGlobalEnvironment( )
{
#ifdef _DEBUG
	g_nSqlHandleCount = 0;
	g_nSqlConnectionCount = 0;
	g_nSqlStatementCount = 0;
	g_nConnectionPoolCount = 0;
	g_nJsConnectionCount = 0;
#endif

	gEnv->pDispatchManager->CreateIoWorkers( );

	if( !gEnv->pOdbcEnv->InitializeEnvironment( ) )
	{
		Nan::ThrowError( gEnv->pOdbcEnv->GetOdbcError( )->ConstructErrorObject( Isolate::GetCurrent( ) ) );
		return false;
	}

	return true;
}

void DestroyGlobalEnvironment( )
{
	gEnv->pDispatchManager.reset( );
	gEnv->pOdbcEnv.reset( );

	assert( g_nSqlHandleCount == 0 );
	assert( g_nSqlConnectionCount == 0 );
	assert( g_nSqlStatementCount == 0 );
	assert( g_nConnectionPoolCount == 0 );
	assert( g_nJsConnectionCount == 0 );

}
