// File: GlobalEnvironment.h
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

#include "Odbc/OdbcEnvironment.h"

#ifdef _DEBUG
#include "Core/Connection/QueryTracker.h"
#include "Core/Connection/ConnectionTracker.h"
#endif // _DEBUG


bool InitializeGlobalEnvironment( );
void DestroyGlobalEnvironment( );


struct SGlobalEnvironment
{
	std::unique_ptr< COdbcEnvironment >		pOdbcEnv = std::make_unique< COdbcEnvironment >( );

#ifdef _DEBUG
	std::unique_ptr< CQueryTracker >		pQueryTracker = std::make_unique< CQueryTracker >( );
	std::unique_ptr< CConnectionTracker >	pConnectionTracker = std::make_unique< CConnectionTracker >( );
#endif // _DEBUG
};

extern SGlobalEnvironment* gEnv;