// File: MssqlExtension.cpp
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
#include "MssqlExtension.h"

#include "Odbc/Helper/Handle/OdbcConnectionHandle.h"


bool CMssqlPoolExtension::PingConnection( const std::shared_ptr< COdbcConnectionHandle > pConnection )
{
	UINT connection_state = SQL_CD_FALSE;
	if( !pConnection->GetConnectionAttribute( SQL_COPT_SS_CONNECTION_DEAD, &connection_state, SQL_IS_UINTEGER, nullptr ) )
	{
		return false;
	}

	return connection_state == SQL_CD_TRUE;
}

bool CMssqlPoolExtension::TestConnectionFeatures( const std::shared_ptr< COdbcConnectionHandle > pConnection )
{
	return true;
}
