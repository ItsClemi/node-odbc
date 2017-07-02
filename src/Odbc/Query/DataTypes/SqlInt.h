#pragma once

#include "DataType.h"

class CSqlInt : public CDataType
{
public:
	CSqlInt( );
	virtual ~CSqlInt( );

public:
	void TransformType( v8::Isolate* isolate, v8::Local< v8::Value > value, SSqlBindParam* pParam );
	bool TransformSqlType( COdbcStatementHandle* pStatement, size_t nColumn, SSqlData* pData );

};