// File: UvOperation.cpp
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

#include "stdafx.h"
#include "UvOperation.h"

#include "Odbc/Query/Query.h"

void CUvOperation::OnInvokeBackground( uv_work_t* req )
{
	auto pOperation = static_cast< CUvOperation* >( req->data );
	{
		pOperation->m_pInstance->ProcessBackground( );
	}
}

void CUvOperation::OnCompleteForeground( uv_work_t* req, int status )
{
	auto isolate = v8::Isolate::GetCurrent( );
	v8::HandleScope scope( isolate );

	auto pOperation = static_cast< CUvOperation* >( req->data );
	{
		auto result = pOperation->m_pInstance->ProcessForeground( isolate );

		if( result == EForegroundResult::eReschedule )
		{
			pOperation->RunOperation( );
		}
		else if( result == EForegroundResult::eDiscard )
		{

			delete( pOperation );
		}
	}
}
