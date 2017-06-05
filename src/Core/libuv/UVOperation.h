// File: UvOperation.h
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


#include "Odbc/Query/Query.h"



class CQuery;
class CUvOperation
{
public:
	CUvOperation( )
	{ 
		m_work.data = this;
	}

	~CUvOperation( )
	{
	}

protected:
	static void OnInvokeBackground( uv_work_t* req );

	static void OnCompleteForeground( uv_work_t* req, int status );

protected: 


public:
	void RunOperation( )
	{
		//> uv_queue_work is not threadsafe!
		assert( v8::Isolate::GetCurrent( ) != nullptr );

		int result = uv_queue_work( uv_default_loop( ), &m_work, OnInvokeBackground, OnCompleteForeground );
		assert( result == 0 );
	}

	void RunOperation( std::shared_ptr< CQuery > pInstance )
	{
		m_pInstance = pInstance;
		RunOperation( );
	}


private:
	std::shared_ptr< CQuery >		m_pInstance;
	uv_work_t						m_work;

};



