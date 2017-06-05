// File: ConnectionPool.h
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

#include "Odbc/Helper/Handle/OdbcConnectionHandle.h"
#include "Extension/PoolExtension.h"


struct SConnectionProps
{
	bool		bEnableMssqlMars;
	uint32_t	nPoolSize;
};

enum class EDriverType : size_t
{
	eGeneric, eMssql, eMySql, eMariaDb, ePostgreSQL, eOracle
};

enum class EPoolState : size_t
{
	eNone, eReady, eReqShutdown, eClosed
};

class CQuery;
class CConnectionPool
{
public:
	typedef std::shared_ptr< COdbcConnectionHandle > TConnection;

public:
	CConnectionPool( );
	~CConnectionPool( );

public:
	bool ReadConnectionProps( v8::Isolate* isolate, v8::Local< v8::Object > value );

	bool ReadResilienceStrategy( v8::Isolate* isolate, v8::Local< v8::Object > value );

public:
	bool InitializePool( const std::wstring& szConnectionString, uint32_t nConnectionTimeout );

private:
	std::shared_ptr< COdbcConnectionHandle > CreateConnection( );

	bool ConnectToServer( TConnection pConnectionOut );

	void DetectDriverType( );

	bool TestConnectionFeatures( std::shared_ptr< COdbcConnectionHandle > pConnection );



public:
	std::shared_ptr< CQuery > CreateQuery( );

	void PushConnection( std::shared_ptr< COdbcConnectionHandle > pConnection );

	void ExecuteQuery( std::shared_ptr< CQuery > pQuery );

	void FinalizeQuery();

private:
	void ResolveDisconnect( );


private:
	std::shared_ptr< COdbcConnectionHandle > PopConnection( )
	{
		if( m_props.bEnableMssqlMars )
		{
			return m_pMarsConnection;
		}
		else
		{
			std::shared_ptr< COdbcConnectionHandle > pConnection;

			m_cs.lock( );
			{
				if( m_queueConnection.size( ) > 0 )
				{
					pConnection = m_queueConnection.front( );
					m_queueConnection.pop( );
				}
			}
			m_cs.unlock( );

// 			for( ;; )
// 			{
// 				if( m_queueConnection.try_pop( pConnection ) )
// 				{
// 					return pConnection;
// 				}
// 
// 				tbb::this_tbb_thread::yield( );
// 			}			

			return nullptr;
		}
	}

public:
	inline void StallQuery( std::shared_ptr< CQuery > pQuery )
	{
		//m_queueStalledQueries.push( pQuery );
	}


public:
	v8::Local< v8::Value > GetConnectionInfo( v8::Isolate* isolate );

	inline const auto GetState( ) const
	{
		return m_eState.load( std::memory_order_relaxed );
	}

private:
	static const auto GetRequiredOdbcApiFunctions( )
	{
		return std::vector< size_t > {
			SQL_API_SQLCONNECT,
				SQL_API_SQLDISCONNECT,
				SQL_API_SQLEXECDIRECT,
				SQL_API_SQLENDTRAN,
				SQL_API_SQLEXECUTE,
				SQL_API_SQLFETCH,
				SQL_API_SQLFREEHANDLE,
				SQL_API_SQLFREESTMT,
				SQL_API_SQLGETDATA,
				SQL_API_SQLPREPARE,
				SQL_API_SQLPUTDATA,
				SQL_API_SQLMORERESULTS,
				SQL_API_SQLBINDPARAM,
		};
	}


public:
	inline void SetSharedInstance( std::shared_ptr< CConnectionPool > pPool )
	{

		m_pPool = pPool;
	}

	inline void RequestDisconnect( v8::Isolate* isolate, v8::Local< v8::Function > fnDisconnect )
	{
		SetState( EPoolState::eReqShutdown );
		m_fnDisconnect.Reset( isolate, fnDisconnect );

		if( m_nPendingQueries == 0 )
		{
			ResolveDisconnect( );
		}
	}

private:
	inline void SetState( EPoolState eState )
	{
		m_eState.store( eState, std::memory_order_relaxed );
	}

public:
	inline bool IsReady( ) const
	{
		return GetState( ) == EPoolState::eReady;
	}

	inline bool IsDead( ) const
	{
		return m_bDead.load( std::memory_order_relaxed );
	}

	inline bool IsResilienceErrorCode( std::shared_ptr< COdbcError > pError )
	{
		for( auto i : m_vecResilienceErrorCodes )
		{
			if( static_cast< SQLUINTEGER >( i ) == pError->GetCode( ) )
			{
				return true;
			}
		}

		return false;
	}

public:
#ifdef _DEBUG
	inline auto GetSharedInstance( )
	{
		assert( !m_pPool.expired( ) );
		return m_pPool.lock( );
	}
#endif


private:
	mutable std::wstring	m_szConnectionString;
	mutable uint32_t		m_nConnectionTimeout;

	SConnectionProps		m_props;
	EDriverType				m_eDriverType = EDriverType::eGeneric;


	std::wstring			m_szDriverName;
	std::wstring			m_szDriverVersion;
	std::wstring			m_szDatabaseName;
	std::wstring			m_szOdbcVersion;
	std::wstring			m_szDBMSName;
	uint64_t				m_nMemoryUsage = 0;
	uint64_t				m_nPerf = 0;
	std::wstring			m_szOdbcConnectionString;

	uint32_t					m_nResilienceRetries = 0;
	std::vector< uint32_t >		m_vecResilienceErrorCodes;


	std::atomic< bool >		m_bDead;

	size_t					m_nPendingQueries = 0;

		
	std::atomic< EPoolState >					m_eState;
	std::unique_ptr< IPoolExtension >			m_pExtension;
	std::weak_ptr< CConnectionPool >			m_pPool;

	v8::Persistent< v8::Function >				m_fnDisconnect;

	std::shared_ptr< COdbcConnectionHandle >	m_pMarsConnection;

	//tbb::concurrent_queue< std::shared_ptr< CQuery > >					m_queueStalledQueries;

	std::mutex		m_cs;
	std::queue< std::shared_ptr< COdbcConnectionHandle > >	m_queueConnection;

	//tbb::concurrent_queue< std::shared_ptr< COdbcConnectionHandle >	>	m_queueConnection;
};