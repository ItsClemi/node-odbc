// File: ParamData.h
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



union UStringData
{
	wchar_t*	pWString;
	char*		pString;
};

enum class EStringType : size_t
{
	eAnsi, eUnicode,
};

struct SStringDesc
{
	void SetString( char* szString, size_t nLen )
	{
		m_eType = EStringType::eAnsi;
		m_nLength = nLen;
		m_stringData.pString = szString;
	}

	void SetString( wchar_t* szString, size_t nLen )
	{
		m_eType = EStringType::eUnicode;
		m_nLength = nLen;
		m_stringData.pWString = szString;
	}

	bool IsAnsiString( )
	{
		return m_eType == EStringType::eAnsi;
	}


	EStringType		m_eType;
	size_t			m_nLength;
	UStringData		m_stringData;
};


struct SBufferDesc
{
	void SetBuffer( uint8_t* pBuffer, size_t nLen )
	{
		m_pBuffer = pBuffer;
		m_nLength = nLen;
	}

	uint8_t*	m_pBuffer;
	size_t		m_nLength;
};


union SParamData
{
	bool						bValue;
	int32_t						nInt32;
	uint32_t					nUint32;
	int64_t						nInt64;
	//float						fValue;
	double						dNumber;

	SBufferDesc					bufferDesc;
	SStringDesc					stringDesc;

	SQL_DATE_STRUCT				sqlDate;
	SQL_TIMESTAMP_STRUCT		sqlTimestamp;
	SQL_NUMERIC_STRUCT			sqlNumeric;
};