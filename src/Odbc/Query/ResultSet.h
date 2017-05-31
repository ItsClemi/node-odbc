// File: ResultSet.h
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

#include "Odbc/Dispatcher/Async/UvJob.h"


struct SMetaData
{
	std::wstring	m_szColumnName;
	SQLULEN			m_nColumnSize;
	SQLSMALLINT		m_nDataType;
	std::wstring	m_szDataTypeName;
	SQLSMALLINT		m_nDecimalDigits;
	SQLSMALLINT		m_bNullable;
	bool			m_bLOBColumn;
};

enum class EResultState
{
	eNone,


	eDrain,
	eDone,
};

struct SResultSetHandler
{
	SResultSetHandler( )
	{

	}

	void Dispose( )
	{
		assert( v8::Isolate::GetCurrent( ) != nullptr );

	}

};


class CQuery;
class CResultSet
{
	enum class EResolveType
	{
		eNone, eCallback, ePromise,
	};

public:	
	CResultSet( CQuery* pQuery );
	virtual ~CResultSet( );

	void Dispose( );

protected:
	bool FetchResults( );

public:
	void ProcessBackground( );

	EForegroundResult ProcessForeground( v8::Isolate* isolate );


private:
	bool DrainRemainingResults( );

protected:
	void Resolve( v8::Isolate* isolate, v8::Local< v8::Value > value );

public:
	inline void SetCallback( v8::Isolate* isolate, v8::Local< v8::Function > fnCallback )
	{
		assert( v8::Isolate::GetCurrent( ) != nullptr );

		m_eResolveType = EResolveType::eCallback;
		m_callback.Reset( isolate, fnCallback );
	}

	inline void SetPromise( v8::Isolate* isolate, v8::Local< v8::Function > fnResolver, v8::Local< v8::Function > fnRejector )
	{
		assert( v8::Isolate::GetCurrent( ) != nullptr );

		m_eResolveType = EResolveType::ePromise;

		m_resolve.Reset( isolate, fnResolver );
		m_reject.Reset( isolate, fnRejector );
	}

private:
	void SetState( EResultState eState )
	{
		m_eState.store( eState, std::memory_order_relaxed );
	}

protected:
	inline EResolveType GetResolveType( ) const
	{
		return m_eResolveType;
	}

	inline EResultState GetState( ) const
	{
		return m_eState.load( std::memory_order_relaxed );
	}

public:
	inline void SetChunkSize( uint32_t nChunkSize )
	{
		m_nChunkSize = nChunkSize;
	}


public:
	inline void EnableMetaData( )
	{

	}


protected:
	bool				m_bExecIsNullData = false;

private:
	mutable CQuery*		m_pQuery;

	uint32_t			m_nChunkSize = 20;
	EResolveType		m_eResolveType = EResolveType::eNone;

	v8::Persistent< v8::Function >		m_callback;
	v8::Persistent< v8::Function >		m_resolve;
	v8::Persistent< v8::Function >		m_reject;



	std::atomic< EResultState >			m_eState;
};