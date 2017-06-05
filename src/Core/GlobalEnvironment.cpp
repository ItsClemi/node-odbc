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


SGlobalEnvironment* gEnv = nullptr;


#ifdef _DEBUG
std::atomic< size_t >	g_nSqlHandleCount;
std::atomic< size_t >	g_nSqlConnectionCount;
std::atomic< size_t >	g_nSqlStatementCount;
std::atomic< size_t >	g_nConnectionPoolCount;
std::atomic< size_t >	g_nJsConnectionCount;
std::atomic< size_t >	g_nActiveQueries;
#endif


bool InitializeGlobalEnvironment( )
{
	gEnv = new SGlobalEnvironment( );

#ifdef _DEBUG
	g_nSqlHandleCount = 0;
	g_nSqlConnectionCount = 0;
	g_nSqlStatementCount = 0;
	g_nConnectionPoolCount = 0;
	g_nJsConnectionCount = 0;
	g_nActiveQueries = 0;
#endif
	
	if( !gEnv->pOdbcEnv->InitializeEnvironment( ) )
	{
		Nan::ThrowError( gEnv->pOdbcEnv->GetOdbcError( )->ConstructErrorObject( Isolate::GetCurrent( ) ) );
		return false;
	}


	return true;
}

void DestroyGlobalEnvironment( )
{
	if( !gEnv )
	{
		//> check if already freed from dtor/node::AtExit
		return;
	}

#ifdef _DEBUG
	gEnv->pQueryTracker = nullptr;
	gEnv->pConnectionTracker->DetachFromJs( );
	gEnv->pConnectionTracker = nullptr;
#endif

	gEnv->pOdbcEnv = nullptr;

	SafeDelete( gEnv );
}
