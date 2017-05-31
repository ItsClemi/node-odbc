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

#include "Async/UvWorker.h"

#include "Odbc/Query/Query.h"
#include "Odbc/Connection/ConnectionPool.h"


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
				while( m_queue.try_pop( pQuery ) )
				{
					if( pQuery->GetPool( )->IsDead( ) )
					{
						pQuery->GetPool( )->StallQuery( pQuery );
						continue;
					}

					EQueryReturn eResult = pQuery->Process( );

					if( eResult == EQueryReturn::eNeedIo )
					{
						PushQuery( pQuery );
					}
					else if( eResult == EQueryReturn::eNeedUv )
					{
						m_nPending.fetch_sub( 1, std::memory_order_relaxed );

						auto pWorker = new CUvWorker< std::shared_ptr< CQuery > >( pQuery );

						pWorker->RunOperation( );
					}
				}
			}

			const auto tmNow = steady_clock::now( );
			const auto tmDelta = ( tmNow - tmStart );

			SetUpdateDelta(  tmDelta );

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