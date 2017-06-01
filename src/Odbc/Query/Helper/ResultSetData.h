// File: ResultSetData.h
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


class CQuery;
class CResultSetData
{
	CResultSetData( std::shared_ptr< CQuery > pQuery );
	~CResultSetData( );

public:


public:
	bool PrepareColumns( size_t nColumns );

private:
	bool ReadColumn( size_t i );


private:
	mutable std::shared_ptr< CQuery >		m_pQuery;


	bool	m_bHasLobColumns = false;


	std::vector< SMetaData, tbb::scalable_allocator< SMetaData > >	m_vecMetaData;
};