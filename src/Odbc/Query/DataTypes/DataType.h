#pragma once
#include "Odbc/Query/Helper/ParamData.h"
#include "Odbc/Query/Helper/JSValue.h"

enum class EInputOutputValue : SQLSMALLINT
{
	eInput = SQL_PARAM_INPUT,
	eOutput = SQL_PARAM_OUTPUT,
};

struct SSqlBindParam
{
	SParamData* GetData( )
	{
		return &m_data;
	}

	ESqlType GetOutputType( )
	{
		return m_eOutputType;
	}

	void Set( SQLSMALLINT nValueType, SQLSMALLINT nParameterType, SQLUINTEGER nColumnSize, SQLSMALLINT nDigits, SQLPOINTER pBuffer, SQLLEN nBufferLen, SQLLEN strLen_or_IndPtr )
	{
		m_nValueType = nValueType;
		m_nParameterType = nParameterType;
		m_nColumnSize = nColumnSize;
		m_nDigits = nDigits;
		m_pBuffer = pBuffer;
		m_nBufferLen = nBufferLen;
		m_strLen_or_IndPtr = strLen_or_IndPtr;
	}

	bool IsInputParam( )
	{
		return m_eInputOutput == EInputOutputValue::eInput;
	}

private:
	EInputOutputValue		m_eInputOutput;
	SQLSMALLINT				m_nValueType;
	SQLSMALLINT				m_nParameterType;
	SQLUINTEGER				m_nColumnSize;
	SQLSMALLINT				m_nDigits;
	SQLPOINTER				m_pBuffer;
	SQLLEN					m_nBufferLen;
	SQLLEN					m_strLen_or_IndPtr;
	SParamData				m_data;

	ESqlType				m_eOutputType;
};

struct SSqlData
{
	void SetNull( )
	{
		m_eType = ESqlType::eNull;
	}


	ESqlType		m_eType;
	SParamData		m_data;
};


class COdbcStatementHandle;
struct CDataType
{
public:
	CDataType( );
	virtual ~CDataType( );

protected:
	//> Serialize
	virtual void TransformType( v8::Isolate* isolate, v8::Local< v8::Value > value, SSqlBindParam* pParam ) = 0;
	//Deserialize
	virtual bool TransformSqlType( COdbcStatementHandle* pStatement, size_t nColumn, SSqlData* pData ) = 0;

protected:
	bool GetData( COdbcStatementHandle* pStatement, SSqlData* pData, size_t nColumn, SQLSMALLINT nTargetType, SQLPOINTER nTargetValue, SQLLEN nBufferLength, SQLLEN* pStrLen = nullptr );



};


