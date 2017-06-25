#pragma once


enum class ESqlType : size_t
{
	eNull,
	eBit, eTinyint, eSmallint, eInt32, eBigInt, eReal, eChar, eNChar, eVarChar,
	eNVarChar, eBinary, eVarBinary, eDate, eTimestamp, eNumeric,

	eLongVarChar, eLongNVarChar, eLongVarBinary,

	eSqlOutputVar,
};


class JSValue
{
public:
	static v8::Local< v8::Value > ToValue( v8::Isolate* isolate, ESqlType eType, SParamData& data )
	{
		v8::EscapableHandleScope scope( isolate );

		v8::Local< v8::Value > value;
		switch( eType )
		{
			case ESqlType::eNull:		value = v8::Null( isolate );													break;
			case ESqlType::eBit:		value = v8::Boolean::New( isolate, data.bValue );								break;
			case ESqlType::eInt32:		value = v8::Int32::New( isolate, data.nInt32 );									break;
			case ESqlType::eBigInt:		value = v8::Number::New( isolate, static_cast< double >( data.nInt64 ) );		break;
			case ESqlType::eReal:		value = v8::Number::New( isolate, data.dNumber );								break;
			case ESqlType::eDate:		value = CJSDate::ToMilliseconds( isolate, data.sqlDate );						break;
			case ESqlType::eTimestamp:	value = CJSDate::ToMilliseconds( isolate, data.sqlTimestamp );					break;
			case ESqlType::eNChar:		value = JSValue::ToString( isolate, data );										break;
			case ESqlType::eNumeric:	value = JSValue::ToNumeric( isolate, data );									break;

				// 			case EJSType::eBinary:
				// 				value = node::Buffer::New( isolate, reinterpret_cast< char* >( m_data.bufferDesc.m_pBuffer ), m_data.bufferDesc.m_nLength ).ToLocalChecked( );
				// 				break;

				// 			case EJSType::eReadStream:
				// 				assert( false );
				// 				break;
			default:
				assert( false );
		}

		return scope.Escape( value );
	}


private:
	static v8::Local< v8::Value > ToString( v8::Isolate* isolate, SParamData& data )
	{
		v8::EscapableHandleScope scope( isolate );
		const auto context = isolate->GetCurrentContext( );

		v8::MaybeLocal< v8::String > string;

		if( data.stringDesc.IsAnsiString( ) )
		{
			string = v8::String::NewFromUtf8( isolate, data.stringDesc.data.pString );
		}
		else
		{
			string = v8::String::NewFromTwoByte(
				isolate,
				reinterpret_cast< const uint16_t* >( data.stringDesc.data.pWString ),
				v8::NewStringType::kNormal
			);
		}

		if( string.IsEmpty( ) )
		{
			return scope.Escape(
				v8::Null( isolate )
			);
		}

		return scope.Escape(
			string.ToLocalChecked( )
		);
	}

	static v8::Local< v8::Value > ToNumeric( v8::Isolate* isolate, SParamData& data )
	{
		v8::EscapableHandleScope scope( isolate );
		const auto context = isolate->GetCurrentContext( );

		auto numeric = v8::Object::New( isolate );

		auto buffer = v8::ArrayBuffer::New( isolate, SQL_MAX_NUMERIC_LEN );
		auto contents = buffer->GetContents( );

		memcpy_s( contents.Data( ), contents.ByteLength( ), data.sqlNumeric.val, SQL_MAX_NUMERIC_LEN );

		if( numeric->Set( context, Nan::New( "precision" ).ToLocalChecked( ), v8::Int32::New( isolate, data.sqlNumeric.precision ) ).IsNothing( ) ||
			numeric->Set( context, Nan::New( "scale" ).ToLocalChecked( ), v8::Int32::New( isolate, data.sqlNumeric.scale ) ).IsNothing( ) ||
			numeric->Set( context, Nan::New( "sign" ).ToLocalChecked( ), v8::Boolean::New( isolate, data.sqlNumeric.sign ) ).IsNothing( ) ||
			numeric->Set( context, Nan::New( "value" ).ToLocalChecked( ), v8::Uint8Array::New( buffer, 0, SQL_MAX_NUMERIC_LEN ) ).IsNothing( )
			)
		{
			return scope.Escape(
				v8::Null( isolate )
			);
		}

		return scope.Escape( numeric );
	}
};