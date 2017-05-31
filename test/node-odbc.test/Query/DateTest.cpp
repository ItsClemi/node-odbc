#include "stdafx.h"
#include "CppUnitTest.h"


#include "Odbc/Query/Helper/JSDate.h"

#pragma push_macro("min")
#pragma push_macro( "max" )

#undef min
#undef max

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace v8;

namespace nodeodbctest
{
	TEST_CLASS( ConnectionTest )
	{
	public:
		TEST_METHOD( ConvertDateToSqlDate )
		{
			//5.27.2017 - 5:07
			int64_t ms = 1495897622040;

			SQL_DATE_STRUCT sqlDate;
			CJSDate::ToSqlDate( ms, sqlDate );

			Assert::AreEqual< int >( sqlDate.day, 27 );
			Assert::AreEqual< int >( sqlDate.month, 5 );
			Assert::AreEqual< int >( sqlDate.year, 2017 );
		}

		TEST_METHOD( ConvertDateToSqlDate2 )
		{
			int64_t ms = 1008555840000;

			SQL_DATE_STRUCT sqlDate;
			CJSDate::ToSqlDate( ms, sqlDate );

			Assert::AreEqual< int >( sqlDate.day, 17 );
			Assert::AreEqual< int >( sqlDate.month, 12 );
			Assert::AreEqual< int >( sqlDate.year, 2001 );
		}


		TEST_METHOD( ConvertNegativeTime )
		{
			//December 17, 1703 03:24:00
			int64_t ms = -8395479360000;

			SQL_DATE_STRUCT sqlDate;
			CJSDate::ToSqlDate( ms, sqlDate );

			Assert::AreEqual< int >( sqlDate.day, 17 );
			Assert::AreEqual< int >( sqlDate.month, 12 );
			Assert::AreEqual< int >( sqlDate.year, 1703 );
		}

		TEST_METHOD( ConvertNegativeTime2 )
		{
			int64_t ms = -17862586560000;

			SQL_DATE_STRUCT sqlDate;
			CJSDate::ToSqlDate( ms, sqlDate );

			Assert::AreEqual< int >( sqlDate.day, 17 );
			Assert::AreEqual< int >( sqlDate.month, 12 );
			Assert::AreEqual< int >( sqlDate.year, 1403 );
		}

		TEST_METHOD( ConvertNegativeTime3 )
		{
			int64_t ms = -46263821760000;

			SQL_DATE_STRUCT sqlDate;
			CJSDate::ToSqlDate( ms, sqlDate );

			Assert::AreEqual< int >( sqlDate.day, 17 );
			Assert::AreEqual< int >( sqlDate.month, 12 );
			Assert::AreEqual< int >( sqlDate.year, 503 );
		}

		TEST_METHOD( ConvertNegativeTime4 )
		{
			int64_t ms = -54247700160000;

			SQL_DATE_STRUCT sqlDate;
			CJSDate::ToSqlDate( ms, sqlDate );

			Assert::AreEqual< int >( sqlDate.day, 17 );
			Assert::AreEqual< int >( sqlDate.month, 12 );
			Assert::AreEqual< int >( sqlDate.year, 250 );
		}

		TEST_METHOD( ConvertZeroTime )
		{
			int64_t ms = 0;

			SQL_DATE_STRUCT sqlDate;
			CJSDate::ToSqlDate( ms, sqlDate );

			Assert::AreEqual< int >( sqlDate.day, 1 );
			Assert::AreEqual< int >( sqlDate.month, 1 );
			Assert::AreEqual< int >( sqlDate.year, 1970 );
		}

		TEST_METHOD( ConvertToTimestamp )
		{	
			//5.27.2017 - 5:07
			int64_t tm = 1495897622040;

			SQL_TIMESTAMP_STRUCT sqlTimestamp;
			CJSDate::ToSqlTimestamp( tm, sqlTimestamp );

			Assert::AreEqual< int >( sqlTimestamp.day, 27 );
			Assert::AreEqual< int >( sqlTimestamp.month, 5 );
			Assert::AreEqual< int >( sqlTimestamp.year, 2017 );
		}

		TEST_METHOD( CheckEqualA )
		{
			SQL_DATE_STRUCT sqlDate;
			{
				sqlDate.year = 2017;
				sqlDate.month = 5;
				sqlDate.day = 27;
			}

			SQL_TIMESTAMP_STRUCT sqlTimestamp = { };
			{
				sqlTimestamp.year = sqlDate.year;
				sqlTimestamp.month = sqlDate.month;
				sqlTimestamp.day = sqlDate.day;
			}
			uint32_t nFraction = 0;
			double dDate = CJSDate::MillisecondsFromDate( sqlTimestamp, &nFraction );


			SQL_DATE_STRUCT sqlRetDate;

			CJSDate::ToSqlDate( static_cast< int64_t >( dDate ), sqlRetDate );

			Assert::IsTrue( memcmp( &sqlDate, &sqlRetDate, sizeof( SQL_DATE_STRUCT ) ) == 0 );
		}

		TEST_METHOD( CheckEqualB )
		{
			SQL_TIMESTAMP_STRUCT sqlTimestamp;
			{
				sqlTimestamp.year = 2350;
				sqlTimestamp.month = 5;
				sqlTimestamp.day = 27;
				sqlTimestamp.hour = 12;
				sqlTimestamp.minute = 32;
				sqlTimestamp.second = 30;
				sqlTimestamp.fraction = 200;
			}

			uint32_t nFraction = 0;
			double dDate = CJSDate::MillisecondsFromDate( sqlTimestamp, &nFraction );


			SQL_TIMESTAMP_STRUCT sqlRetDate;

			CJSDate::ToSqlTimestamp( static_cast< int64_t >( dDate ), sqlRetDate );

			Assert::IsTrue( memcmp( &sqlTimestamp, &sqlRetDate, sizeof( SQL_DATE_STRUCT ) ) == 0 );
		}



	};

}

#pragma pop_macro( "min" )
#pragma pop_macro( "max" )
