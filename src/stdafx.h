// File: stdafx.h
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

//////////////////////////////////////////////////////////////////////////
//> Defines

#if defined( _WIN32 ) || defined( _WIN64 )

#define WIN32_LEAN_AND_MEAN

#ifdef _DEBUG
#define _WIN_DBG
#endif
#endif

#ifdef _MSC_VER

#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__			__FUNCTION__

#ifndef __PRETTY_FUNCTIONW__
#define __PRETTY_FUNCTIONW__		__FUNCTIONW__
#endif

#endif



#endif

#ifndef __PRETTY_FUNCTIONW__
#define __PRETTY_FUNCTIONW__		__PRETTY_FUNCTION__
#endif


#define TBB_PREVIEW_MEMORY_POOL 1

//////////////////////////////////////////////////////////////////////////

#ifdef _WINDOWS

#include <SDKDDKVer.h>
#include <windows.h>

#ifdef _DEBUG
#include <crtdbg.h>  
#endif

#endif


#ifdef _DEBUG
#include <cassert>
#else
#define assert(expression) ((void)0)
#endif

#include <uv.h>
#include <nan.h>
#include <node.h>
#include <node_buffer.h>
#include <node_object_wrap.h>

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <sqlucode.h>

#include <memory>
#include <vector>
#include <atomic>
#include <string>
#include <iostream>
#include <sstream>
#include <codecvt>
#include <future>
#include <algorithm>
#include <array>
#include <chrono>
#include <condition_variable>
#include <queue>


#include <tbb/tbb.h>
#include <tbb/scalable_allocator.h>
#include <tbb/memory_pool.h>


#include "Odbc/Drivers/msodbcsql.h"

#include "Common/Helper/SharedPtrHolder.h"
#include "Common/Helper/Utility.h"
#include "Common/Helper/NodeExt.h"

#include "Core/GlobalEnvironment.h"

#include "Core/libuv/UvOperation.h"


