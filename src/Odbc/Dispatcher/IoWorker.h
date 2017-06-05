// File: IoWorker.h
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


class CQuery;
class CIoWorker
{
public:
	CIoWorker( );
	~CIoWorker( );

public:
	void Run( );

	void PushQuery( const std::shared_ptr< CQuery >& pQuery );

private:
	static void _ResolveUv( uv_async_t* pAsync );
		

	auto PopQuery( )
	{
		std::lock_guard< std::mutex > l( m_cs2 );

		std::shared_ptr< CQuery > pQuery;

		if( !m_queue.empty( ) )
		{
			pQuery = m_queue.front( );
			m_queue.pop( );
		}

		return pQuery;
	}

public:
	inline bool IsYield( )
	{
		return m_bYield.load( std::memory_order_relaxed );
	}

	inline void WakeWorker( )
	{
		m_cv.notify_all( );
	}

public:
	inline auto GetUpdateDelta( ) const
	{
		return std::chrono::nanoseconds(
			m_tmUpdateDelta.load( std::memory_order::memory_order_relaxed )
		);
	}

private:
	std::atomic< long long >			m_tmUpdateDelta;
	std::atomic< size_t >				m_nProcessedQueries;
	std::atomic< bool >					m_bYield;
	std::atomic< bool >					m_bHighLoad;
	std::atomic< bool >					m_bShutdown;
	std::atomic< size_t >				m_nPending;

	std::thread							m_thread;
	std::condition_variable				m_cv;
	std::mutex							m_cs;


	//tbb::concurrent_queue< std::shared_ptr< CQuery > >			m_queue;

	std::mutex		m_cs2;
	std::queue< std::shared_ptr< CQuery > >	m_queue;

};