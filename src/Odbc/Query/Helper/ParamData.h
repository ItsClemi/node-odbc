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
		data.pString = szString;
	}

	void SetString( wchar_t* szString, size_t nLen )
	{
		m_eType = EStringType::eUnicode;
		m_nLength = nLen;
		data.pWString = szString;
	}

	bool IsAnsiString( )
	{
		return m_eType == EStringType::eAnsi;
	}

	void Alloc( EStringType eType, size_t nLength )
	{
		m_eType = eType;
		m_nLength = nLength;

		if( IsAnsiString( ) )
		{
			data.pString = new char[ nLength + 1 ];
		}
		else
		{
			data.pWString = new wchar_t[ nLength + 1 ];
		}
	}

	void Dispose( )
	{
		if( IsAnsiString( ) )
		{
			SafeDeleteArray( data.pString );
		}
		else
		{
			SafeDeleteArray( data.pWString );
		}
	}

	wchar_t* GetWString( )
	{
		assert( m_eType == EStringType::eUnicode );
		return data.pWString;
	}

	char* GetString( )
	{
		assert( m_eType == EStringType::eAnsi );
		return data.pString;
	}


	EStringType		m_eType;
	size_t			m_nLength;
	UStringData		data;
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

enum class EBufferType
{
	eWstring, eString, eBinary
};

struct SDataBuffer
{
	wchar_t* AllocWstring( size_t nLength )
	{
		data.pWstring = new wchar_t[ nLength + 1 ];
		length = nLength;

		return data.pWstring;
	}

	char* AllocString( size_t nLength )
	{
		data.pString = new char[ nLength + 1 ];
		length = nLength;

		return data.pString;
	}

	void DestroyWString( )
	{

	}

	void DestroyString( )
	{

	}

	void DestroyBuffer( )
	{
		delete[ ] data.pBuffer;
	}


	EBufferType		type;
	size_t			length;

	union
	{
		wchar_t*	pWstring;
		char*		pString;
		uint8_t*	pBuffer;
	} data;
};


union SParamData
{
	bool						bValue;
	int32_t						nInt32;
	uint32_t					nUint32;
	int64_t						nInt64;
	double						dNumber;

	SBufferDesc					bufferDesc;
	SStringDesc					stringDesc;
	SDataBuffer					dataBuffer;

	SQL_DATE_STRUCT				sqlDate;
	SQL_TIMESTAMP_STRUCT		sqlTimestamp;
	SQL_NUMERIC_STRUCT			sqlNumeric;
};