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

#include "Helper/ColumnData.h"


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


enum class EFetchMode : size_t
{
	eSingle,
	eArray,

	eMax,
	eNone,
};


enum class EResultState
{
	eNone,

	ePrepareFetch,

	eDrain,
	eDone,
};

enum class EFecthResult : size_t
{
	eError,
	eDone,
	eHasMoreData
};

enum class EFoundResultSet
{
	eError,
	eEmpty,
	eSuccess,
};


class CQuery;
class CResultSet
{
	static const size_t sDefaultChunkSize = 5;

	enum class EResolveType
	{
		eNone, eCallback, ePromise
	};

public:	
	CResultSet( CQuery* pQuery );
	virtual ~CResultSet( );

public:
	bool FetchResults( );

private:
	bool PrepareColumns( );
	
	bool FindNextResultSet( );

	EFecthResult FetchChunk( size_t nChunkSize, size_t* nFetched );

	bool ReadColumn( size_t nColumn );

	bool GetSqlData( size_t ColumnNumber, SQLSMALLINT TargetType, SQLPOINTER TargetValue, SQLLEN BufferLength, SQLLEN *StrLen_or_IndPtr );


public:
	void Resolve( v8::Isolate* isolate, v8::Local< v8::Value > value );

	v8::Local< v8::Value > ConstructResult( v8::Isolate* isolate );

	v8::Local< v8::Value > ConstructResultRow( v8::Isolate* isolate, int nRow );

	void AddResultExtensions( v8::Isolate* isolate, v8::Local< v8::Object > value );

	void AddMetaDataExtension( v8::Isolate* isolate, v8::Local< v8::Object > value );

	void AddReturnValueExtension( v8::Isolate* isolate, v8::Local< v8::Object > value );

	void AddQueryInstanceExtension( v8::Isolate* isolate, v8::Local< v8::Object > value );

public:
	void SetPromise( v8::Isolate* isolate, v8::Local< v8::Function > fnResolver, v8::Local< v8::Function > fnRejector );

private:
	void SetState( EResultState eState )
	{
		m_eState.store( eState, std::memory_order_relaxed );
	}

	void SetError( );

public:
	inline const EResolveType GetResolveType( ) const
	{
		return m_eResolveType;
	}

	inline const EResultState GetState( ) const
	{
		return m_eState.load( std::memory_order_relaxed );
	}

	inline const EFetchMode GetFetchMode( ) const
	{
		return m_eFetchMode;
	}

	inline auto GetMetaData( size_t nColumn )
	{
		return &m_vecMetaData[ nColumn ];
	}

	inline auto GetColumnData( size_t nRow, size_t nColumn )
	{
		return &m_vecData[ ( nRow * m_nColumns ) + nColumn ];
	}

	inline auto GetColumnData( size_t nColumn )
	{
		return GetColumnData( m_nActiveRow, nColumn );
	}

public:
	inline void SetFetchMode( EFetchMode eFetchMode )
	{
		m_eFetchMode = eFetchMode;
	}

	inline void SetCallback( v8::Isolate* isolate, v8::Local< v8::Function > fnCallback )
	{
		assert( v8::Isolate::GetCurrent( ) != nullptr );

		m_eResolveType = EResolveType::eCallback;
		m_callback.Reset( isolate, fnCallback );
	}

	inline void SetPromise( )
	{
		m_eResolveType = EResolveType::ePromise;
	}



public:
	inline void EnableMetaData( )
	{
		m_bEnableMetaData = true;
	}

public:
	inline bool IsMetaDataEnabled( ) const
	{
		return m_bEnableMetaData;
	}

public:
	bool					m_bExecNoData = false;
	mutable EFetchMode		m_eFetchMode = EFetchMode::eNone;

	size_t					m_nMemoryUsage = 0;

	v8::Persistent< v8::Value >				m_queryInstance;

private:
	mutable CQuery*			m_pQuery;
	EResolveType			m_eResolveType = EResolveType::eNone;

	bool					m_bEnableMetaData = false;

	SQLSMALLINT				m_nColumns = 0;
	size_t					m_nActiveRow = 0;
	bool					m_bHasLobColumns = false;



	v8::Persistent< v8::Function >			m_callback;
	v8::Persistent< v8::Function >			m_resolve;
	v8::Persistent< v8::Function >			m_reject;

	v8::Persistent< v8::Value >				m_result;


	std::atomic< EResultState >				m_eState;

	std::vector< SMetaData/*, tbb::scalable_allocator< SMetaData >*/ >			m_vecMetaData;
	std::vector< SColumnData/*, tbb::scalable_allocator< SColumnData >*/ >		m_vecData;
};