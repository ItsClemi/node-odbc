// File: Query.h
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

#include "Odbc/Helper/Handle/OdbcStatementHandle.h"

#include "Odbc/Query/QueryParameter.h"
#include "Odbc/Query/ResultSet.h"

#include "Odbc/Dispatcher/Async/UvJob.h"


enum class EQueryState : size_t
{
	eNone,
	eExecuteStatement,
	
	eNeedData,
	eFetchResult,

	eEnd
};


enum class EQueryReturn : size_t
{
	//used if CQuery pushes data to JS (bucket data fetching)
	eNeedIo,

	eNeedUv,
};


class CConnectionPool;
class COdbcConnectionHandle;
class CQuery : public COdbcStatementHandle, public CQueryParameter, public CResultSet, public IUvJob
{
public:
	CQuery( const std::shared_ptr< CConnectionPool > pPool );
	virtual ~CQuery( );

public:
	EQueryReturn Process( );

public:
	virtual void ProcessBackground( ) override;
	virtual EForegroundResult ProcessForeground( v8::Isolate* isolate ) override;

private:
	bool ExecuteStatement( );
	bool BindOdbcParameters( );

public:
	void SetError( );

public:
	inline void InitializeQuery( EFetchMode eMode, const std::wstring& szQuery ) const
	{
		m_eFetchMode = eMode;
		m_szQuery = szQuery;
	}

	inline void InitializeQuery( const std::wstring& szQuery ) const
	{
		m_eFetchMode = EFetchMode::eNone;
		m_szQuery = szQuery;
	}

public:
	inline const std::wstring& GetQueryString( ) const
	{
		return m_szQuery;
	}

	inline const EFetchMode GetFetchMode( ) const
	{
		return m_eFetchMode;
	}

	inline EQueryState GetState( ) const
	{
		return m_eState.load( std::memory_order_relaxed );
	}

	inline const std::shared_ptr< CConnectionPool > GetPool( ) const
	{
		return m_pPool;
	}

public:
	inline bool HasError( ) const
	{
		return m_pError != nullptr;
	}

	inline bool IsIdle( ) const
	{
		return (GetState( ) == EQueryState::eNone || GetState( ) == EQueryState::eEnd);
	}

	inline bool IsActive( ) const
	{
		return !IsIdle( );
	}



private:
	inline void SetState( EQueryState eState ) const
	{
		m_eState.store( eState, std::memory_order_relaxed );
	}

public:
	inline void SetConnection( std::shared_ptr< COdbcConnectionHandle > pConnection )
	{
		m_pConnection = pConnection;
	}

	inline void SetQueryTimeout( uint32_t nTimeout )
	{
		if( nTimeout > 1000 )
		{
			EnableSlowQuery( );
		}

		m_nQueryTimeout = static_cast< SQLUINTEGER >( nTimeout );
	}

	inline void SetFetchMode( EFetchMode eFetchMode )
	{
		m_eFetchMode = eFetchMode;
	}



public:
	inline bool SetParameters( v8::Isolate* isolate, EFetchMode eFetchMode, const std::wstring szQuery, const Nan::FunctionCallbackInfo<v8::Value>& args, const int nPos, const v8::Local< v8::Function > fnCallback )
	{
		SetCallback( isolate, fnCallback );

		return SetParameters( isolate, eFetchMode, szQuery, args, nPos );
	}

	inline bool SetParameters( v8::Isolate* isolate, EFetchMode eFetchMode, const std::wstring szQuery, const Nan::FunctionCallbackInfo<v8::Value>& args, const int nPos )
	{
		InitializeQuery( eFetchMode, szQuery );

		return BindParameters( isolate, args, nPos );
	}

public:
	inline void EnableReturnValue( )
	{

	}

	inline void EnableSlowQuery( )
	{
		m_bSlowQuery = true;
	}

	inline void EnableTransaction( )
	{

	}



private:
	const std::shared_ptr< CConnectionPool >	m_pPool;
	std::shared_ptr< COdbcConnectionHandle >	m_pConnection;

	mutable std::wstring						m_szQuery;
	mutable EFetchMode							m_eFetchMode;

	mutable std::atomic< EQueryState >			m_eState;


	std::shared_ptr< COdbcError >				m_pError;

	bool			m_bSlowQuery = false;
	bool			m_bHasReturnValue = false;
	SQLUINTEGER		m_nQueryTimeout = 0;
	SQLSMALLINT		m_nReturnValue = 0;
	SQLLEN			m_nCbReturnValue = 0;

	size_t			m_nRetries = 0;
	
};