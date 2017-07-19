#pragma once

#include "DataType.h"


class CSqlBigInt : public CDataType
{
public:
	CSqlBigInt( );
	virtual ~CSqlBigInt( );

public:
	void TransformType( v8::Isolate* isolate, v8::Local< v8::Value > value, SSqlBindParam* pParam );
	bool TransformSqlType( COdbcStatementHandle* pStatement, size_t nColumn, SSqlData* pData );

};