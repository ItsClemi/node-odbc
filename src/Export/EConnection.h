// File: EConnection.h
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


#include "Odbc/Connection/ConnectionPool.h"

class EConnection : public Nan::ObjectWrap
{
	static Nan::Persistent< v8::Function >	constructor;

public:
	static NAN_MODULE_INIT( InitializeModule );

private:
	static NAN_METHOD( New );

	static NAN_METHOD( Connect );
	static NAN_METHOD( Disconnect );

	static NAN_METHOD( SetResilienceStrategy );
	static NAN_METHOD( PrepareQuery );
	static NAN_METHOD( ExecuteQuery );
	static NAN_METHOD( GetInfo );


private:
	EConnection( );
	virtual ~EConnection( );

public:
	inline auto GetPool( ) const
	{
		return m_pPool;
	}

private:
	std::shared_ptr< CConnectionPool >	m_pPool = std::make_shared< CConnectionPool >( );

};