#pragma once

#include "Odbc/Query/DataTypes/DataType.h"


class CSqlReal : public CDataType
{
public:
	CSqlReal( );
	virtual ~CSqlReal( );

protected:
	virtual void Serialize( v8::Isolate* isolate, v8::Local<v8::Value> value, SSqlBindParam* pParam ) override;
	virtual bool Deserialize( COdbcStatementHandle* pStatement, size_t nColumn, SMetaData* pMetaData, SSqlData* pData ) override;

};