// File: ColumnData.h
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

#include "ParamData.h"
#include "JSDate.h"
#include "JSValue.h"


struct SColumnData
{
	void Dispose( )
	{
		if( m_eType == ESqlType::eNChar )
		{
			DisposeString( );
		}
		else if( m_eType == ESqlType::eBinary )
		{
			delete[ ] m_data.bufferDesc.m_pBuffer;
			//scalable_free( m_data.bufferDesc.m_pBuffer );
		}
	}

	void SetNull( )
	{
		m_eType = ESqlType::eNull;
	}

	void SetBool( bool bValue )
	{
		m_eType = ESqlType::eBit;
		m_data.bValue = bValue;
	}

	void SetInt32( int32_t nInt32 )
	{
		m_eType = ESqlType::eInt32;
		m_data.nInt32 = nInt32;
	}

	void SetInt64( int64_t nInt64 )
	{
		m_eType = ESqlType::eBigInt;
		m_data.nInt64 = nInt64;
	}

	void SetDouble( double dValue )
	{
		m_eType = ESqlType::eReal;
		m_data.dNumber = dValue;
	}

	void SetDate( SQL_DATE_STRUCT& sqlDate )
	{
		m_eType = ESqlType::eDate;
		m_data.sqlDate = sqlDate;
	}

	void SetTimestamp( SQL_TIMESTAMP_STRUCT& sqlTimestamp )
	{
		m_eType = ESqlType::eTimestamp;
		m_data.sqlTimestamp = sqlTimestamp;
	}

	void SetNumeric( SQL_NUMERIC_STRUCT& sqlNumeric )
	{
		m_eType = ESqlType::eNumeric;
		m_data.sqlNumeric = sqlNumeric;
	}

	void SetBuffer( uint8_t* pBuffer, size_t nLength )
	{
		m_eType = ESqlType::eBinary;
		m_data.bufferDesc.SetBuffer( pBuffer, nLength );
	}

	void SetLobColumn( )
	{
		m_eType = ESqlType::eLongVarBinary;
	}


	void SetStringLength( size_t nLength )
	{
		m_eType = ESqlType::eNChar;
		m_data.stringDesc.m_nLength = nLength;
	}

	UStringData& AllocString( EStringType eType, size_t nLength )
	{
		m_data.stringDesc.Alloc( eType, nLength );

		return m_data.stringDesc.data;
	}

	void DisposeString( )
	{
		assert( m_eType == ESqlType::eNChar );

		m_data.stringDesc.Dispose( );
	}

public:
	v8::Local< v8::Value > ToValue( v8::Isolate* isolate )
	{
		return JSValue::ToValue( isolate, m_eType, m_data );
	}

private:
	ESqlType		m_eType;
	SParamData		m_data;
};