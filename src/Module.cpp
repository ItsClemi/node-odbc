// File: Module.cpp
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

#include "Export/EConnection.h"
#include "Export/EModuleHelper.h"
#include "Export/EPreparedQuery.h"


using namespace v8;


void InitializeModule( Local< Object > exports )
{
#ifdef _WIN_DBG
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_DELAY_FREE_MEM_DF );
	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_DEBUG );
	_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_DEBUG );
#endif
	
	if( !InitializeGlobalEnvironment( ) )
	{ 
		return;
	}

	EConnection::InitializeModule( exports );
	EModuleHelper::InitializeModule( exports );
	EPreparedQuery::InitializeModule( exports );

	node::AtExit( [ ]( void* args )
	{
		DestroyGlobalEnvironment( );

#ifdef _WIN_DBG
		_CrtCheckMemory( );

		if( _CrtDumpMemoryLeaks( ) == TRUE )
		{
			__debugbreak( );
		}
#endif
	} );
}

#ifdef _WINDOWS
BOOL WINAPI DllMain( _In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved )
{
	return TRUE;
}
#endif

//fix clang visual studio link bug
#if defined __clang__ && defined( _MSC_VER )

#undef NODE_C_CTOR

#define NODE_C_CTOR(fn)                                               \
  NODE_CTOR_PREFIX void fn(void) __attribute__((constructor));        \
  NODE_CTOR_PREFIX void fn(void)

#endif

NODE_MODULE( nodeodbc, InitializeModule )
