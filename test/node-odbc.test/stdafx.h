#pragma once

#define MSSQL_CON	L"Driver={SQL Server Native Client 11.0};Server=DESKTOP-OA6HU8M;Database=node-odbc-test;Uid=sa;Pwd=apfelbrot;" 

#define __PRETTY_FUNCTION__ __FUNCTION__
#define __TEST_BUILD

#include <Winsock2.h>

#include <SDKDDKVer.h>
#include <windows.h>

#include <crtdbg.h>  


#include "CppUnitTest.h"


#include <cassert>

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
#include <limits>

#include <tbb/concurrent_hash_map.h>
#include <tbb/concurrent_queue.h>
#include <tbb/scalable_allocator.h>
#include <tbb/task.h>
#include <tbb/concurrent_vector.h>

#include "Odbc/Drivers/msodbcsql.h"


#include "Common/Helper/Utility.h"
#include "Common/Helper/NodeExt.h"
