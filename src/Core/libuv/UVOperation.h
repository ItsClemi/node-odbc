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


class CUvOperation
{
public:
	CUvOperation( )
	{
		m_work.data = this;
	}

	virtual ~CUvOperation( )
	{ }

protected:
	virtual void OnInvokeBackground( ) = 0;
	virtual void OnCompleteForeground( ) = 0;

private:
	static void OnBackground( uv_work_t* work )
	{
		const auto pOperation = reinterpret_cast< CUvOperation* >( work->data );
		{
			pOperation->OnInvokeBackground( );
		}
	}

	static void OnForeground( uv_work_t* work, int nStatus )
	{
		const auto pOperation = reinterpret_cast< CUvOperation* >( work->data );
		{
			pOperation->OnCompleteForeground( );
		}
	}

public:
	inline void RunOperation( )
	{
		int res = uv_queue_work( uv_default_loop( ), &m_work, OnBackground, OnForeground );
		assert( res == 0 );
	}

private:
	uv_work_t		m_work;
};