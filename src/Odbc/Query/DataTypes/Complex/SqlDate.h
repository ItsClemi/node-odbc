#pragma once


#include "Odbc/Query/DataTypes/DataType.h"


class CSqlDate : public CDataType
{
public:
	CSqlDate( );
	virtual ~CSqlDate( );

public:

	virtual void Serialize( v8::Isolate* isolate, v8::Local< v8::Value > value, SSqlBindParam* pParam ) override;
	virtual bool Deserialize( COdbcStatementHandle* pStatement, size_t nColumn, SMetaData* pMetaData, SSqlData* pData ) override;


};