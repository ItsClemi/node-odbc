#pragma once

#include "Odbc/Query/DataTypes/DataType.h"


class CSqlBigInt : public CDataType
{
public:
	CSqlBigInt( );
	virtual ~CSqlBigInt( );

public:
	void Serialize( v8::Isolate* isolate, v8::Local< v8::Value > value, SSqlBindParam* pParam );
	bool Deserialize( COdbcStatementHandle* pStatement, size_t nColumn, SMetaData* pMetaData, SSqlData* pData );

};