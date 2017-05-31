// File: Utility.h
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

template< typename T >
inline void SafeDelete( T& t )
{
	if( t != nullptr )
	{
		delete( t );
		t = nullptr;
	}
}

template< typename T >
inline void SafeDeleteArray( T& t )
{
	if( t != nullptr )
	{
		delete[ ]( t );
		t = nullptr;
	}
}




inline std::wstring StringToWString( const char* szStr )
{
	std::wstring_convert< std::codecvt_utf8< wchar_t > > converter;
	return converter.from_bytes( szStr );
}

inline std::string WStringToString( const wchar_t* szStr )
{
	std::wstring_convert< std::codecvt_utf8< wchar_t > > converter;
	return converter.to_bytes( szStr );
}


inline const std::wstring FromV8String( const v8::Local< v8::String > str )
{
	const int nLength = str->Length( );

	std::wstring szResult;
	
	if( nLength > 0 )
	{
		szResult.resize( static_cast< size_t >( nLength ) );

		str->Write( reinterpret_cast< uint16_t* >( &szResult.at( 0 ) ) );
	}

	return szResult;
}

inline const v8::Local< v8::String > ToV8String( v8::Isolate* isolate, const std::wstring& szStr )
{
	v8::EscapableHandleScope scope( isolate );

	const auto str = v8::String::NewFromTwoByte(
		isolate,
		reinterpret_cast< const uint16_t* >( szStr.c_str( ) ),
		v8::NewStringType::kNormal,
		static_cast< int >( szStr.length( ) )
	).ToLocalChecked( );

	return scope.Escape( str );
}

inline const v8::Local< v8::String > ToV8String( const std::wstring& szStr )
{
	return ToV8String( v8::Isolate::GetCurrent( ), szStr );
}


inline void ThrowV8Exception( v8::Isolate* isolate, const std::wstring& szException )
{
	v8::HandleScope scope( isolate );

	auto szMessage = v8::String::NewFromTwoByte(
		isolate,
		reinterpret_cast< const uint16_t* >( szException.c_str( ) ),
		v8::NewStringType::kNormal,
		static_cast< int >( szException.length( ) )
	);

	isolate->ThrowException( szMessage.ToLocalChecked( ) );
}

inline void ThrowV8Exception( v8::Isolate* isolate, const std::wostringstream& stream )
{
	ThrowV8Exception( isolate, stream.str( ) );
}

inline void ThrowV8Exception( v8::Isolate* isolate, const wchar_t* szException )
{
	ThrowV8Exception( isolate, std::wstring( szException ) );
}




template< typename E >
constexpr auto ToUnderlyingType( E e ) noexcept
{
	return static_cast< std::underlying_type_t< E > >( e );
}


#ifndef V8_TYPE_VALIDATE
#define V8_TYPE_VALIDATE( exp, err ) \
	if( ( exp ) == false ) \
	{ \
		Nan::ThrowTypeError( Nan::New( err ).ToLocalChecked( ) ); \
		return; \
	}
#endif

#ifndef V8_RUNTIME_VALIDATE
#define V8_RUNTIME_VALIDATE( exp, err ) \
	if( ( exp ) == false ) \
	{ \
		Nan::ThrowError( Nan::New( err ).ToLocalChecked( ) ); \
		return; \
	} 
#endif


