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

#include "Core/libuv/UvOperation.h"


#include "Core/libuv/IUvOperation.h"

enum class EQueryState : size_t
{
	eNone,
	eExecuteStatement,
	
	eNeedData,
	eFetchResult,

	eEnd
};


class CConnectionPool;
class COdbcConnectionHandle;
class CQuery : public IUvOperation
{
public:
	CQuery( const std::shared_ptr< CConnectionPool > pPool );
	~CQuery( );
	

public:
	virtual void ProcessBackground( ) override;

	virtual EForegroundResult ProcessForeground( v8::Isolate* isolate ) override;

private:
	void ProcessQuery( );

	bool ExecuteStatement( );
	bool BindOdbcParameters( );

public:
	void SetError( );


public:
	inline void InitializeQuery( EFetchMode eMode, const std::wstring& szQuery )
	{
		GetResultSet( )->m_eFetchMode = eMode;
		m_szQuery = szQuery;
	}

	inline void InitializeQuery( const std::wstring& szQuery )
	{
		InitializeQuery( EFetchMode::eNone, szQuery );
	}

public:
	inline const std::wstring& GetQueryString( ) const
	{
		return m_szQuery;
	}

	inline EQueryState GetState( ) const
	{
		return m_eState.load( std::memory_order_relaxed );
	}

	inline const std::shared_ptr< CConnectionPool > GetPool( ) const
	{
		return m_pPool;
	}

	inline const int32_t GetReturnValue( )
	{
		return static_cast< int32_t >( m_nReturnValue );
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

	inline bool IsReturnValueEnabled( ) const
	{
		return m_bReturnValue;
	}

private:
	inline void SetState( EQueryState eState )
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


public:
	inline bool SetParameters( v8::Isolate* isolate, EFetchMode eFetchMode, const v8::Local< v8::Function > fnCallback, const std::wstring szQuery, const Nan::FunctionCallbackInfo<v8::Value>& args, const int nPos )
	{
		GetResultSet()->SetCallback( isolate, fnCallback );

		return SetParameters( isolate, eFetchMode, szQuery, args, nPos );
	}

	inline bool SetParameters( v8::Isolate* isolate, EFetchMode eFetchMode, const std::wstring szQuery, const Nan::FunctionCallbackInfo<v8::Value>& args, const int nPos )
	{
		InitializeQuery( eFetchMode, szQuery );

		return GetQueryParam()->BindParameters( isolate, args, nPos );
	}

	
public:
	inline void EnableReturnValue( )
	{

	}

	inline void EnableSlowQuery( )
	{
		m_bSlowQuery = true;
	}

	inline void EnableTransaction( v8::Isolate* isolate, v8::Local< v8::Value > instance )
	{
		GetResultSet()->m_queryInstance.Reset( isolate, instance );
		m_bTransactionEnabled = true;
	}

public:
	COdbcStatementHandle* GetStatement( )
	{
		return &m_statement;
	}

	CQueryParameter* GetQueryParam( )
	{
		return &m_queryParam;
	}

	CResultSet* GetResultSet( )
	{
		return &m_resultSet;
	}


private:
	const std::shared_ptr< CConnectionPool >	m_pPool;
	std::shared_ptr< COdbcConnectionHandle >	m_pConnection;

	std::wstring								m_szQuery;

	std::atomic< EQueryState >					m_eState;


	COdbcStatementHandle	m_statement;
	CQueryParameter			m_queryParam;
	CResultSet				m_resultSet;


	std::shared_ptr< COdbcError >				m_pError;

	bool			m_bSlowQuery = false;
	bool			m_bReturnValue = false;
	bool			m_bTransactionEnabled = false;


	SQLUINTEGER		m_nQueryTimeout = 0;
	SQLSMALLINT		m_nReturnValue = 0;
	SQLLEN			m_nCbReturnValue = 0;

};