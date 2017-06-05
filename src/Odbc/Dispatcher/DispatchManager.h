
// File: DispatchManager.h
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


class CQuery;
class CIoWorker;
class CDispatchManager
{
public:
	CDispatchManager( );
	~CDispatchManager( );

public:
	void CreateIoWorkers( );

private:
	CIoWorker* GetIoWorker( );

public:
	void PushQuery( const std::shared_ptr< CQuery >& pQuery );

public:


private:
	size_t			m_nWorkerCount;


	//used to distribute queries across the worker threads equally
	size_t			m_nDispatchCount = 0;


	std::vector< CIoWorker* >		m_vecWorker;


};