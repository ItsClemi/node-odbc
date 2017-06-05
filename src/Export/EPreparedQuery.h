// File: EPreparedQuery.h
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
class EPreparedQuery : public Nan::ObjectWrap
{
	static Nan::Persistent< v8::Function >	constructor;

public:
	static NAN_MODULE_INIT( InitializeModule );

public:
	static v8::Local< v8::Object > CreateInstance( v8::Isolate* isolate, const std::shared_ptr< CQuery > pQuery );

private:
	static NAN_METHOD( New );

	static NAN_METHOD( EnableReturnValue );
	static NAN_METHOD( EnableMetaData );
	static NAN_METHOD( EnableSlowQuery );
	static NAN_METHOD( EnableTransaction );

	static NAN_METHOD( SetQueryTimeout );
	
	static NAN_METHOD( ToSingle );
	static NAN_METHOD( ToArray );


	static NAN_METHOD( Rollback );
	static NAN_METHOD( Commit );

	static NAN_METHOD( SetPromiseInfo );


private:
	EPreparedQuery( );
	virtual ~EPreparedQuery( );

public:
	inline auto GetQuery( ) const
	{
		return m_pQuery;
	}

private:

	inline void SetQuery( const std::shared_ptr< CQuery > pQuery ) 
	{
		m_pQuery = pQuery;
	}

private:
	std::shared_ptr< CQuery >		m_pQuery;

};