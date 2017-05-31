#include "stdafx.h"

#include "Core/GlobalEnvironment.h"
#include "Odbc/OdbcEnvironment.h"
#include <libplatform/libplatform.h>


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace v8;

namespace nodeodbctest
{
	TEST_MODULE_INITIALIZE( InitEnv )
	{
		gEnv->pOdbcEnv = std::make_unique< COdbcEnvironment >( );

		Assert::IsTrue( gEnv->pOdbcEnv->InitializeEnvironment( ) );
	}

	TEST_MODULE_CLEANUP( DestroySqlEnv )
	{
		gEnv->pOdbcEnv.reset( nullptr );
	}

}