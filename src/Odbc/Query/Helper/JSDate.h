//---------------------------------------------------------------------------------------------------------------------------------
// File: JSDate.h
// Contents: Javascript <-> SQL_DATE
// 
// Copyright Microsoft Corporation and contributors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
//
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//---------------------------------------------------------------------------------------------------------------------------------
//src: https://github.com/Azure/node-sqlserver/blob/master/src/Column.cpp

// Copyright 2012 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//src: https://github.com/v8/v8/blob/master/src/date.cc

#pragma once



class CJSDate
{
	static const int64_t NANOSECONDS_PER_MS = 1000000;
	static const int64_t MS_PER_SECOND = 1000;
	static const int64_t MS_PER_MINUTE = 60 * MS_PER_SECOND;
	static const int64_t MS_PER_HOUR = 60 * MS_PER_MINUTE;
	static const int64_t MS_PER_DAY = 24 * MS_PER_HOUR;


	static const int kMsPerMin = 60 * 1000;
	static const int kSecPerDay = 24 * 60 * 60;
	static const int64_t kMsPerDay = kSecPerDay * 1000;
	static const int64_t kMsPerMonth = kMsPerDay * 30;

	static const int kDaysIn4Years = 4 * 365 + 1;
	static const int kDaysIn100Years = 25 * kDaysIn4Years - 1;
	static const int kDaysIn400Years = 4 * kDaysIn100Years + 1;
	static const int kDays1970to2000 = 30 * 365 + 7;
	static const int kDaysOffset = 1000 * kDaysIn400Years + 5 * kDaysIn400Years - kDays1970to2000;
	static const int kYearsOffset = 400000;


public:
	inline static void ToSqlDate( int64_t tmMs, SQL_DATE_STRUCT& sqlDate )
	{
		int year, day, month, hour, min, sec, ms;
		uint32_t fraction;
		BreakDownTime( tmMs, &year, &month, &day, &hour, &min, &sec, &ms, &fraction );
		{
			sqlDate.year = static_cast< SQLSMALLINT >( year );
			sqlDate.month = static_cast< SQLUSMALLINT >( month );
			sqlDate.day = static_cast< SQLUSMALLINT >( day );
		}
	}

	inline static void ToSqlTimestamp( int64_t tmMs, SQL_TIMESTAMP_STRUCT& sqlTimestamp )
	{
		int year, day, month, hour, min, sec, ms;
		uint32_t fraction;

		BreakDownTime( tmMs, &year, &month, &day, &hour, &min, &sec, &ms, &fraction );		
		{
			sqlTimestamp.year = static_cast< SQLSMALLINT >( year );
			sqlTimestamp.month = static_cast< SQLSMALLINT >( month );
			sqlTimestamp.day = static_cast< SQLSMALLINT >( day );
			sqlTimestamp.hour = static_cast< SQLSMALLINT >( hour );
			sqlTimestamp.minute = static_cast< SQLSMALLINT >( min );
			sqlTimestamp.second = static_cast< SQLSMALLINT >( sec );
			sqlTimestamp.fraction = static_cast< SQLUINTEGER >( fraction );
			assert( sqlTimestamp.fraction >= 0 && sqlTimestamp.fraction <= 999900000 );
		}
	}

	inline static v8::Local< v8::Value > ToMilliseconds( v8::Isolate* isolate, const SQL_DATE_STRUCT& sqlDate )
	{
		SQL_TIMESTAMP_STRUCT sqlTimestamp = {};
		{ 
			sqlTimestamp.year = sqlDate.year;
			sqlTimestamp.month = sqlDate.month;
			sqlTimestamp.day = sqlDate.day;
		}
		return ToMilliseconds( isolate, sqlTimestamp );
	}


	inline static v8::Local< v8::Value > ToMilliseconds( v8::Isolate* isolate, const SQL_TIMESTAMP_STRUCT& sqlTimestamp )
	{
		v8::EscapableHandleScope scope( isolate );
		const auto context = isolate->GetCurrentContext( );

		uint32_t nFraction = 0;
		auto date = v8::Date::New( context, MillisecondsFromDate( sqlTimestamp, &nFraction ) ).ToLocalChecked( );
		
		if( !date.As< v8::Date >( )->Set(
			Nan::New( "nanosecondsDelta" ).ToLocalChecked( ),
			v8::Number::New( isolate, static_cast< double >( nFraction ) / ( static_cast< double >( NANOSECONDS_PER_MS ) * 1000.0 ) ) )
			)
		{
			Nan::ThrowError( Nan::New( "failed to construct date object!" ).ToLocalChecked() );
			return scope.Escape( v8::Undefined( isolate ) );
		}
	
		return scope.Escape( date );
	}


#ifndef __TEST_BUILD
private:
#endif
	inline static void BreakDownTime( int64_t time_ms, int* year, int* month, int* day, int* hour, int* min, int* sec, int* ms, uint32_t* fraction )
	{
		int const days = DaysFromTime( time_ms );
		int const time_in_day_ms = TimeInDay( time_ms, days );
		YearMonthDayFromDays( days, year, month, day );

		//sql month range 1 - 12
		//see: https://docs.microsoft.com/en-us/sql/relational-databases/native-client-odbc-date-time/data-type-support-for-odbc-date-and-time-improvements
		*month += 1;


		*hour = time_in_day_ms / ( 60 * 60 * 1000 );
		*min = ( time_in_day_ms / ( 60 * 1000 ) ) % 60;
		*sec = ( time_in_day_ms / 1000 ) % 60;
		*ms = time_in_day_ms % 1000;

		*fraction = static_cast< uint32_t >( ( ( time_in_day_ms ) % 1000 ) * NANOSECONDS_PER_MS );
	}

	inline static void YearMonthDayFromDays( int days, int* year, int* month, int* day )
	{
		static const char kDaysInMonths[ ] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

		int save_days = days;

		days += kDaysOffset;
		*year = 400 * ( days / kDaysIn400Years ) - kYearsOffset;
		days %= kDaysIn400Years;

		assert( save_days == DaysFromYearMonth( *year, 0 ) + days );

		days--;
		int yd1 = days / kDaysIn100Years;
		days %= kDaysIn100Years;
		*year += 100 * yd1;

		days++;
		int yd2 = days / kDaysIn4Years;
		days %= kDaysIn4Years;
		*year += 4 * yd2;

		days--;
		int yd3 = days / 365;
		days %= 365;
		*year += yd3;


		bool is_leap = ( !yd1 || yd2 ) && !yd3;

		assert( days >= -1 );
		assert( is_leap || ( days >= 0 ) );
		assert( ( days < 365 ) || ( is_leap && ( days < 366 ) ) );
		assert( is_leap == ( ( *year % 4 == 0 ) && ( *year % 100 || ( *year % 400 == 0 ) ) ) );
		assert( is_leap || ( ( DaysFromYearMonth( *year, 0 ) + days ) == save_days ) );
		assert( !is_leap || ( ( DaysFromYearMonth( *year, 0 ) + days + 1 ) == save_days ) );

		days += is_leap;

		// Check if the date is after February.
		if( days >= 31 + 28 + BoolToInt( is_leap ) )
		{
			days -= 31 + 28 + BoolToInt( is_leap );
			// Find the date starting from March.
			for( int i = 2; i < 12; i++ )
			{
				if( days < kDaysInMonths[ i ] )
				{
					*month = i;
					*day = days + 1;
					break;
				}
				days -= kDaysInMonths[ i ];
			}
		}
		else
		{
			// Check January and February.
			if( days < 31 )
			{
				*month = 0;
				*day = days + 1;
			}
			else
			{
				*month = 1;
				*day = days - 31 + 1;
			}
		}

		assert( DaysFromYearMonth( *year, *month ) + *day - 1 == save_days );
	}

	static int DaysFromYearMonth( int year, int month )
	{
		static const int day_from_month[ ] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
		static const int day_from_month_leap[ ] = { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 };

		year += month / 12;
		month %= 12;
		if( month < 0 )
		{
			year--;
			month += 12;
		}

		assert( month >= 0 );
		assert( month < 12 );

		// year_delta is an arbitrary number such that:
		// a) year_delta = -1 (mod 400)
		// b) year + year_delta > 0 for years in the range defined by
		//    ECMA 262 - 15.9.1.1, i.e. upto 100,000,000 days on either side of
		//    Jan 1 1970. This is required so that we don't run into integer
		//    division of negative numbers.
		// c) there shouldn't be an overflow for 32-bit integers in the following
		//    operations.
		static const int year_delta = 399999;
		static const int base_day = 365 * ( 1970 + year_delta ) +
			( 1970 + year_delta ) / 4 -
			( 1970 + year_delta ) / 100 +
			( 1970 + year_delta ) / 400;

		int year1 = year + year_delta;
		int day_from_year = 365 * year1 +
			year1 / 4 -
			year1 / 100 +
			year1 / 400 -
			base_day;

		if( ( year % 4 != 0 ) || ( year % 100 == 0 && year % 400 != 0 ) )
		{
			return day_from_year + day_from_month[ month ];
		}

		return day_from_year + day_from_month_leap[ month ];
	}

	inline static int BoolToInt( bool b )
	{
		return b ? 1 : 0;
	}

	inline static int DaysFromTime( int64_t time_ms )
	{
		if( time_ms < 0 )
		{
			time_ms -= ( kMsPerDay - 1 );
		}

		return static_cast< int >( time_ms / kMsPerDay );
	}

	inline static int TimeInDay( int64_t time_ms, int days )
	{
		return static_cast< int >( time_ms - days * kMsPerDay );
	}



	inline static bool is_leap_year( int64_t year )
	{
		return ( ( ( year % 4 == 0 ) && ( year % 100 != 0 ) ) || ( year % 400 ) == 0 );
	}


	inline static double DaysSinceEpoch( SQLSMALLINT y, SQLUSMALLINT m, SQLUSMALLINT d )
	{
		// table derived from ECMA 262 15.9.1.4
		static const double days_in_months[ ] = { 0.0, 31.0, 59.0, 90.0, 120.0, 151.0, 181.0, 212.0, 243.0, 273.0, 304.0, 334.0 };

		double days;

		// calculate the number of days to the start of the year
		days = 365.0 * ( y - 1970.0 ) + floor( ( y - 1969.0 ) / 4.0 ) - floor( ( y - 1901.0 ) / 100.0 ) + floor( ( y - 1601.0 ) / 400.0 );

		// add in the number of days from the month

		assert( m > 0 && m < 12 );
		days += days_in_months[ m - 1 ];

		// and finally add in the day from the date to the number of days elapsed
		days += d - 1.0;

		// account for leap year this year (affects days after Feb. 29)
		if( is_leap_year( y ) && m > 2 )
		{
			days += 1.0;
		}

		return floor( days );
	}

	inline static double MillisecondsFromDate( const SQL_TIMESTAMP_STRUCT& timeStruct, uint32_t* fraction )
	{
		double ms = DaysSinceEpoch( timeStruct.year, timeStruct.month, timeStruct.day );
		ms *= MS_PER_DAY;

		// add in the hour, day minute, second and millisecond
		ms += timeStruct.hour * MS_PER_HOUR + timeStruct.minute * MS_PER_MINUTE + timeStruct.second * MS_PER_SECOND;
		ms += timeStruct.fraction / NANOSECONDS_PER_MS;    // fraction is in nanoseconds

		*fraction = timeStruct.fraction % static_cast< uint32_t >( NANOSECONDS_PER_MS );
		return ms;
	}
};

/*
  if (!std::isnan(time_val)) {
	int64_t const time_ms = static_cast<int64_t>(time_val);
	int64_t local_time_ms = isolate->date_cache()->ToLocal(time_ms);
	int day = isolate->date_cache()->DaysFromTime(local_time_ms);
	int time_within_day = isolate->date_cache()->TimeInDay(local_time_ms, day);
	int h = time_within_day / (60 * 60 * 1000);
	int m = (time_within_day / (60 * 1000)) % 60;
	int s = (time_within_day / 1000) % 60;
	time_val = MakeDate(day, MakeTime(h, m, s, ms->Number()));
  }

*/