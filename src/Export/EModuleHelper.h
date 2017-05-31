// File: EModuleHelper.h
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


class EModuleHelper
{
public:
	static v8::Persistent< v8::Function >	s_fnWriteStreamInitializer;
	static v8::Persistent< v8::Function >	s_fnReadStreamInitializer;
	static v8::Persistent< v8::Function >	s_fnPromiseInitializer;

public:
	static void InitializeModule( const v8::Local< v8::Object > exports );

public:
	static void SetWriteStreamInitializer( const v8::FunctionCallbackInfo< v8::Value >& args );
	static void SetReadStreamInitializer( const v8::FunctionCallbackInfo< v8::Value >& args );
	static void SetPromiseInitializer( const v8::FunctionCallbackInfo< v8::Value >& args );

	static void ProcessNextChunk( const v8::FunctionCallbackInfo< v8::Value >& args );
	static void RequestNextChunk( const v8::FunctionCallbackInfo< v8::Value >& args );


public:
	static const v8::Local< v8::Value > CreatePromise( v8::Isolate* isolate, const v8::Local< v8::Value > _this );

};