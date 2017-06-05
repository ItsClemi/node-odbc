// File: OdbcEnvironment.cpp
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

#include "stdafx.h"
#include "OdbcEnvironment.h"

#include "Odbc/Helper/OdbcError.h"


COdbcEnvironment::COdbcEnvironment( )
{ }

COdbcEnvironment::~COdbcEnvironment( )
{ }

bool COdbcEnvironment::InitializeEnvironment( )
{
	//SetEnvironmentAttribute( SQL_ATTR_CONNECTION_POOLING, ( SQLPOINTER )SQL_CP_DRIVER_AWARE );
	
	if( !AllocHandle( ) )
	{
		return false;
	}

	if( !SetEnvironmentAttribute( SQL_ATTR_ODBC_VERSION, ( SQLPOINTER )SQL_OV_ODBC3_80 ) )
	{
		if( !SetEnvironmentAttribute( SQL_ATTR_ODBC_VERSION, ( SQLPOINTER )SQL_OV_ODBC3 ) )
		{
			return false;
		}
	}

	//SetEnvironmentAttribute( SQL_ATTR_CP_MATCH, ( SQLPOINTER )SQL_CP_RELAXED_MATCH );

	return true;
}
