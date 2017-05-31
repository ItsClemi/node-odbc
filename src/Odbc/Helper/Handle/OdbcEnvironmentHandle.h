// File: OdbcEnvironmentHandle.h
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

#include "Odbc/Helper/Handle/OdbcHandle.h"


class COdbcEnvironmentHandle : public TOdbcHandle< eOdbcHandleType::eSqlHandleEnv >
{
public:
	COdbcEnvironmentHandle( )
	{ }

	virtual ~COdbcEnvironmentHandle( )
	{ }
	
public:
	inline bool SetEnvironmentAttribute( SQLINTEGER nAttribute, SQLPOINTER pValue, SQLINTEGER nStringLength = 0 )
	{
		return VALIDATE_SQL_RESULT( SQLSetEnvAttr( GetSqlHandle( ), nAttribute, pValue, nStringLength ) );
	}

	inline bool GetEnvironmentAttribute( SQLINTEGER Attribute, SQLPOINTER ValuePtr, SQLINTEGER BufferLength, SQLINTEGER* StringLengthPtr )
	{
		return VALIDATE_SQL_RESULT( SQLGetEnvAttr( GetSqlHandle( ), Attribute, ValuePtr, BufferLength, StringLengthPtr ) );
	}



};