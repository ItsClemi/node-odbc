// File: UvWorker.h
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

#include "Core/libuv/UvOperation.h"
#include "UvJob.h"


class CQuery;
class CConnectionPool;

template< typename T >
class CUvWorker : public CUvOperation
{
public:
	CUvWorker( T pWork )
		: m_pWork( pWork )
	{
	}

	virtual ~CUvWorker( )
	{
	}

protected:
	virtual void OnInvokeBackground( ) override
	{
		m_pWork->ProcessBackground( );
	}

	virtual void OnCompleteForeground( ) override
	{
		auto isolate = v8::Isolate::GetCurrent( );

		v8::HandleScope scope( isolate );

		EForegroundResult eResult = m_pWork->ProcessForeground( isolate );

		if( eResult == EForegroundResult::eReschedule )
		{
			RunOperation( );
		}
		else if( eResult == EForegroundResult::eDiscard )
		{
			delete( this );
		}
	}


protected:
	T		m_pWork;
};
