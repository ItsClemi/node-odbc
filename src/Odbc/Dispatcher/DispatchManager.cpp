// File: DispatchManager.cpp
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
#include "DispatchManager.h"
#include "IoWorker.h"

#include <tbb/task_scheduler_init.h>


using namespace v8;


CDispatchManager::CDispatchManager( )
{
	//we can spawn a lot of threads because odbc is a blocking protocol
	m_nWorkerCount = tbb::task_scheduler_init::default_num_threads( ) * 2;

}

CDispatchManager::~CDispatchManager( )
{
	for( auto i : m_vecWorker )
	{
		SafeDelete( i );
	}
	m_vecWorker.clear( );

}

void CDispatchManager::CreateIoWorkers( )
{
	for( size_t i = 0; i < m_nWorkerCount; i++ )
	{
		const auto pWorker = new CIoWorker;
		{
			pWorker->Run( );
		}
		m_vecWorker.push_back( pWorker );
	}
}

CIoWorker* CDispatchManager::GetIoWorker( )
{
	return m_vecWorker[ m_nDispatchCount++ % m_nWorkerCount ];
}

void CDispatchManager::PushQuery( const std::shared_ptr< CQuery >& pQuery )
{
	assert( Isolate::GetCurrent( ) != nullptr );


	const auto pWorker = GetIoWorker( );

	pWorker->PushQuery( pQuery );

	if( pWorker->IsYield( ) )
	{
		pWorker->WakeWorker( );
	}
}
