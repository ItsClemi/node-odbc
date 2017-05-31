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

struct SBufferDesc
{
	uint8_t*	pBuffer;
	size_t		nLen;
};


enum class EStringType : size_t
{
	eAnsi, eUnicode,
};

struct SStringDesc
{
	EStringType		eType;
	size_t			nLen;

	union
	{
		wchar_t*	pWString;
		char*		pString;
	} stringData;
};


union SParamData
{
	bool			bValue;
	int32_t			nValue;
	uint32_t		unValue;
	int64_t			llValue;
	float			fValue;
	double			dValue;

	SBufferDesc		m_buffer;
	SStringDesc		m_string;

	SQL_DATE_STRUCT					m_sqlDate;
	SQL_TIMESTAMP_STRUCT			m_sqlTimestamp;
	SQL_NUMERIC_STRUCT				m_sqlNumeric;
};