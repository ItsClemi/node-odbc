// File: IoWorker.cpp
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
#include "IoWorker.h"
#include "Odbc/Query/Query.h"

using namespace v8;
using namespace std::chrono;

CIoWorker::CIoWorker( )
{
	m_bYield.store( true, std::memory_order_relaxed );
	m_bHighLoad.store( false, std::memory_order_relaxed );
	m_nProcessedQueries.store( 0, std::memory_order_relaxed );
	m_bShutdown.store( false, std::memory_order_relaxed );
	m_nPending.store( 0, std::memory_order::memory_order_relaxed );
}

CIoWorker::~CIoWorker( )
{
	if( m_thread.joinable( ) )
	{
		m_bShutdown.store( true, std::memory_order_relaxed );
		m_thread.join( );
	}
}



void CIoWorker::Run( )
{
	m_thread = std::thread( [ this ]
	{
		for( ;; )
		{
			if( m_bShutdown.load( std::memory_order_relaxed ) )
			{
				assert( m_nPending == 0 );
				break;
			}

			std::unique_lock< std::mutex > lk( m_cs );

			const auto tmStart = steady_clock::now( );


			{
				std::shared_ptr< CQuery > pQuery;

				for( ;; )
				{
					pQuery = PopQuery( );

					if( !pQuery )
					{
						break;
					}

					auto eProcessResult = pQuery->ProcessQuery( );

					if( eProcessResult == EQueryResult::eError )
					{
						pQuery->SetError( );
					}
					else if( eProcessResult == EQueryResult::eDone )
					{
						//..
					}


					uv_async_send( &pQuery->m_async );
				}
			}

			const auto tmNow = steady_clock::now( );
			const auto tmDelta = ( tmNow - tmStart );

			//SetUpdateDelta( tmDelta );

			if( tmDelta < 1s )
			{
				if( m_bHighLoad.load( std::memory_order_relaxed ) )
				{
					m_bHighLoad.store( false, std::memory_order_relaxed );
				}

				//#TODO check for work to steal

				m_bYield.store( true, std::memory_order_relaxed );
				{
					m_cv.wait_for( lk, 1s - tmDelta );
				}
				m_bYield.store( false, std::memory_order_relaxed );
			}
			else
			{
				m_bHighLoad.store( true, std::memory_order_relaxed );
			}

		}
	} );
}

void CIoWorker::_ResolveUv( uv_async_t* pAsync )
{

	__debugbreak( );
	//auto pQueryAsync = reinterpret_cast< uv_query_async_t* >( pAsync );

// 	auto isolate = Isolate::GetCurrent( );
// 	assert( isolate != nullptr );
// 	HandleScope scope( isolate );

	//pQueryAsync->pQuery->ProcessForeground( isolate );

// 	auto pPQuery = reinterpret_cast< std::shared_ptr< CQuery >* >( pAsync->data );
// 
// 
// 
// 	auto pOperation = new CUvOperation;
// 	
// 	pOperation->RunOperation( *pPQuery );
// 
// 
// 	pPQuery->_Decref( );

	pAsync->async_sent = true;
	uv_close( ( uv_handle_t* )pAsync, nullptr );
}



void CIoWorker::PushQuery( const std::shared_ptr< CQuery >& pQuery )
{
	assert( v8::Isolate::GetCurrent( ) != nullptr );

	//pQuery->m_async.pIoWorker = this;
	//pQuery->m_async.pQuery = pQuery;

	pQuery->m_async.data = new std::shared_ptr< CQuery >( pQuery );


	int n = uv_async_init( uv_default_loop( ), &pQuery->m_async, _ResolveUv );


	std::lock_guard< std::mutex >l( m_cs2 );

	m_nPending.fetch_add( 1, std::memory_order_relaxed );
	m_queue.push( pQuery );
}



// 				std::shared_ptr< CQuery > pQuery;
// 				while( m_queue.try_pop( pQuery ) )
// 				{
// // 					if( pQuery->GetPool( )->IsDead( ) )
// // 					{
// // 						pQuery->GetPool( )->StallQuery( pQuery );
// // 						continue;
// // 					}
// // 
// // 					EQueryReturn eResult = pQuery->Process( );
// // 
// // 					if( eResult == EQueryReturn::eNeedIo )
// // 					{
// // 						PushQuery( pQuery );
// // 					}
// // 					else if( eResult == EQueryReturn::eNeedUv )
// // 					{
// // 						m_nPending.fetch_sub( 1, std::memory_order_relaxed );
// // 
// // 						auto pWorker = new CUvWorker< std::shared_ptr< CQuery > >( pQuery );
// // 
// // 						pWorker->RunOperation( );
// // 					}
// 				}