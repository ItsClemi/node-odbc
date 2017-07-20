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

	uint8_t* AllocBuffer( size_t nLength )
	{
		data.pBuffer = new uint8_t[ nLength ];
		length = nLength;

		return data.pBuffer;
	}

	void DestroyWString( )
	{
		delete[ ] data.pWstring;
	}

	void DestroyString( )
	{
		delete[ ] data.pString;
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

	SDataBuffer					dataBuffer;

	SQL_DATE_STRUCT				sqlDate;
	SQL_TIMESTAMP_STRUCT		sqlTimestamp;
	SQL_NUMERIC_STRUCT			sqlNumeric;
};