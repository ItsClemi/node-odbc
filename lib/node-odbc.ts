// File: node-odbc.ts
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


import * as stream from "stream";
import * as fs from "fs";
import * as bluebird from "bluebird";
import * as binding from "./binding";

export type SqlComplexType = {
	readonly _typeId: number;
};

export type SqlStream = SqlComplexType & {
	stream: stream.Readable | stream.Writable;
	length: number;
};

export type SqlNumeric = SqlComplexType & {
	precision: number;
	scale: number;
	sign: boolean;
	value: Uint8Array;
};

export type SqlTimestamp = SqlComplexType & {
	date: Date;
}

export type SqlOutputParameter = SqlComplexType & {
	ref: any;
	paramType: eSqlOutputType;

	length?: number;
	precision?: number;
	scale?: number;
}

export type SqlError = {
	readonly message: string;
	readonly sqlState: string;
	readonly code: number;
};

export type SqlColumnMetaData = {
	readonly name: string;
	readonly size: number;
	readonly dataType: string;
	readonly digits: number;
	readonly nullable: boolean;
};


export type SqlResultExtension = {
	readonly $sqlReturnValue?: number;
	readonly $sqlMetaData?: Array<SqlColumnMetaData>;
};

export type SqlResult = SqlResultExtension & Partial<any>;

export type SqlPartialResult<T> = SqlResultExtension & Partial<T>;


export type SqlResultArray = SqlResultExtension & Array<any>;

export type SqlPartialResultArray<T> = SqlResultExtension & Array<T>;


export type SqlResultTypes = SqlResult | SqlResultArray;

export type SqlPartialResultTypes<T> = SqlPartialResult<T> & SqlPartialResultArray<T>;


export type SqlTypes = null | string | boolean | number | Date | Buffer | SqlStream | SqlNumeric | SqlTimestamp | SqlOutputParameter;


export interface IResilienceStrategy
{
	retries: number;
	errorCodes: Array<number>;
}

export type ConnectionInfo = {
	driverName: string;
	driverVersion: string;
	databaseName: string;
	odbcVersion: string;
	dbmsName: string;
	internalServerType: number;
	odbcConnectionString: string;
	resilienceStrategy: IResilienceStrategy;
};

export type ConnectionProps = {
	enableMssqlMars?: boolean;
	poolSize?: number;
};

export const enum eFetchMode
{
	eSingle,
	eArray,
};

export const enum eSqlOutputType
{
	eBitOutput,
	eTinyintOutput,
	eSmallint,
	eInt,
	eUint32,
	eBigInt,
	eFloat,
	eReal,

	eChar,
	eNChar,
	eVarChar,
	eNVarChar,

	eBinary,
	eVarBinary,

	eDate,
	eTimestamp,

	eNumeric,
}


export declare class Connection 
{
	constructor( advancedProps?: ConnectionProps );

	connect( connectionString: string, connectionTimeout?: number ): Connection;

	disconnect( cb: () => void ): void;

	prepareQuery( query: string, ...args: ( SqlTypes )[] ): ISqlQuery;


	executeQuery( cb: ( result: SqlResult, error: SqlError ) => void, query: string, ...args: ( SqlTypes )[] ): void;

	executeQuery( eFetchMode: eFetchMode, cb: ( result: SqlResultTypes, error: SqlError ) => void, query: string, ...args: ( SqlTypes )[] ): void;

	executeQuery<T>( cb: ( result: SqlPartialResult<T>, error: SqlError ) => void, query: string, ...args: ( SqlTypes )[] ): void;

	executeQuery<T>( eFetchMode: eFetchMode, cb: ( result: SqlPartialResultTypes<T>, error: SqlError ) => void, query: string, ...args: ( SqlTypes )[] ): void;


	getInfo(): ConnectionInfo;
}

export const enableValidation = true;



declare interface IRawConnection
{
	connect( connectionString: string, connectionTimeout?: number ): Connection;

	disconnect( cb: () => void ): void;

	prepareQuery( query: string, ...args: ( SqlTypes )[] ): ISqlQuery;


	executeQuery( cb: ( result: SqlResult, error: SqlError ) => void, query: string, ...args: ( SqlTypes )[] ): void;

	executeQuery( eFetchMode: eFetchMode, cb: ( result: SqlResultTypes, error: SqlError ) => void, query: string, ...args: ( SqlTypes )[] ): void;

	executeQuery<T>( cb: ( result: SqlPartialResult<T>, error: SqlError ) => void, query: string, ...args: ( SqlTypes )[] ): void;

	executeQuery<T>( eFetchMode: eFetchMode, cb: ( result: SqlPartialResultTypes<T>, error: SqlError ) => void, query: string, ...args: ( SqlTypes )[] ): void;


	getInfo(): ConnectionInfo;
}

export class Connection2
{
	private _connection: IRawConnection;

	constructor( advancedProps?: ConnectionProps )
	{
		if( enableValidation )
		{
			if( advancedProps )
			{
				if( ( advancedProps.enableMssqlMars != undefined && typeof ( advancedProps.enableMssqlMars ) != "boolean" ) ||
					( advancedProps.poolSize != undefined && typeof ( advancedProps.poolSize ) != "number" )
				)
				{
					throw new TypeError( "advancedProps contains invalid type" );
				}
			}
		}

		this._connection = new exports.Connection( advancedProps ) as IRawConnection;
	}

	public connect( connectionString: string, connectionTimeout?: number ): Connection2
	{
		if( enableValidation )
		{
			if( typeof ( connectionString ) != "string" )
			{
				throw new TypeError( `connectionString: invalid type ${connectionString}` );
			}

			if( connectionTimeout != undefined && typeof ( connectionTimeout ) != "number" )
			{
				throw new TypeError( "" );
			}
		}

		this._connection.connect( connectionString, connectionTimeout );


		return this;
	}

}



export interface ISqlQuery 
{
	enableReturnValue(): ISqlQuery;

	enableMetaData(): ISqlQuery;


	setQueryTimeout( timeout: number ): ISqlQuery;


	toSingle( cb: ( result: SqlResult, error: SqlError ) => void ): void;

	toSingle<T>( cb: ( result: SqlPartialResult<T>, error: SqlError ) => void ): void;


	toSingle(): Promise<SqlResult>;

	toSingle<T>(): Promise<SqlPartialResult<T>>;


	toArray( cb: ( result: SqlResultArray, error: SqlError ) => void ): void;

	toArray<T>( cb: ( result: SqlPartialResultArray<T>, error: SqlError ) => void ): void;


	toArray(): Promise<SqlResultArray>;

	toArray<T>(): Promise<SqlPartialResultArray<T>>;
}

export interface ISqlQueryEx extends ISqlQuery
{
	setPromiseInfo( resolve, reject ): void;
}




//> type helpers
const ID_INPUT_STREAM: number = 0;
const ID_NUMERIC_VALUE: number = 1;
const ID_DATE_VALUE: number = 2;
const ID_OUTPUT_PARAMETER: number = 3;


export function makeInputStream( stream: fs.ReadStream | stream.Readable, length: number ): SqlStream
{
	return { _typeId: ID_INPUT_STREAM, stream, length };
}

export function makeNumeric( precision: number, scale: number, sign: boolean, value: Uint8Array ): SqlNumeric
{
	return { _typeId: ID_NUMERIC_VALUE, precision, scale, sign, value };
}

export function makeTimestamp( date: Date ): SqlTimestamp
{
	return { _typeId: ID_DATE_VALUE, date };
}


export const SqlOutput = {

	asBitOutput( ref: boolean ): SqlOutputParameter
	{
		return { _typeId: ID_OUTPUT_PARAMETER, paramType: eSqlOutputType.eBitOutput, ref };
	},

	asTinyint( ref: number ): SqlOutputParameter
	{
		return { _typeId: ID_OUTPUT_PARAMETER, paramType: eSqlOutputType.eTinyintOutput, ref };
	},

	asSmallint( ref: number ): SqlOutputParameter
	{
		return { _typeId: ID_OUTPUT_PARAMETER, paramType: eSqlOutputType.eSmallint, ref };
	},

	asInt( ref: number ): SqlOutputParameter
	{
		return { _typeId: ID_OUTPUT_PARAMETER, paramType: eSqlOutputType.eInt, ref };
	},

	asBigInt( ref: number ): SqlOutputParameter
	{
		return { _typeId: ID_OUTPUT_PARAMETER, paramType: eSqlOutputType.eBigInt, ref };
	},

	asFloat( ref: number ): SqlOutputParameter
	{
		return { _typeId: ID_OUTPUT_PARAMETER, paramType: eSqlOutputType.eFloat, ref };
	},

	asReal( ref: number ): SqlOutputParameter
	{
		return { _typeId: ID_OUTPUT_PARAMETER, paramType: eSqlOutputType.eReal, ref };
	},


	asChar( ref: string, length: number ): SqlOutputParameter
	{
		return { _typeId: ID_OUTPUT_PARAMETER, paramType: eSqlOutputType.eChar, ref, length };
	},

	asNChar( ref: string, length: number ): SqlOutputParameter
	{
		return { _typeId: ID_OUTPUT_PARAMETER, paramType: eSqlOutputType.eNChar, ref, length };
	},

	asVarChar( ref: string, length: number ): SqlOutputParameter
	{
		return { _typeId: ID_OUTPUT_PARAMETER, paramType: eSqlOutputType.eVarChar, ref, length };
	},

	asNVarChar( ref: string, length: number ): SqlOutputParameter
	{
		return { _typeId: ID_OUTPUT_PARAMETER, paramType: eSqlOutputType.eNVarChar, ref, length };
	},


	asBinary( ref: Uint8Array, length: number ): SqlOutputParameter
	{
		return { _typeId: ID_OUTPUT_PARAMETER, paramType: eSqlOutputType.eBinary, ref, length };
	},

	asVarBinary( ref: Uint8Array, length: number ): SqlOutputParameter
	{
		return { _typeId: ID_OUTPUT_PARAMETER, paramType: eSqlOutputType.eVarBinary, ref, length };
	},


	asDate( ref: Date, scale: number ): SqlOutputParameter
	{
		return { _typeId: ID_OUTPUT_PARAMETER, paramType: eSqlOutputType.eDate, ref, scale };
	},

	asTimestamp( ref: Date, scale: number ): SqlOutputParameter
	{
		return { _typeId: ID_OUTPUT_PARAMETER, paramType: eSqlOutputType.eTimestamp, ref, scale };
	},


	asNumeric( ref: SqlNumeric, precision: number, scale: number ): SqlOutputParameter
	{
		return { _typeId: ID_OUTPUT_PARAMETER, paramType: eSqlOutputType.eTimestamp, ref, precision, scale };
	},
};



/*
	internal js/C++ bridge helpers

	see docs for more information.
	
	WARNING: you may cause undefined behaviour by changing these
*/

export interface IJSBridge
{
	setWriteStreamInitializer( cb: ( targetStream: stream.Readable, query: ISqlQueryEx ) => void ): void;

	setPromiseInitializer<T>( cb: ( query: ISqlQueryEx ) => T ): void;

	setReadStreamInitializer( cb: ( query: ISqlQueryEx, column: number ) => stream.Readable ): void;

	processNextChunk( query: ISqlQueryEx, chunk: Int8Array, cb: ( error ) => void ): void;
	requestNextChunk( query: ISqlQueryEx, column: number, cb: ( chunk: Int8Array ) => void ): Int8Array;
}

export function getJSBridge(): IJSBridge
{
	return exports as IJSBridge;
}

class SqlStreamReader extends stream.Readable
{
	private query: ISqlQueryEx;
	private column: number;

	constructor( _query: ISqlQueryEx, _column: number )
	{
		super();

		this.query = _query;
		this.column = _column;
	}

	public _read()
	{
		getJSBridge().requestNextChunk( this.query, this.column, ( chunk ) =>
		{
			this.push( chunk );
		} );
	}
}

class SqlStreamWriter extends stream.Writable
{
	private _query: ISqlQueryEx;

	constructor( query: ISqlQueryEx )
	{
		super();

		this._query = query;
	}

	public _write( chunk, encoding: string, next: Function )
	{
		getJSBridge().processNextChunk( this._query, chunk, ( error ) =>
		{
			next( error );
		} );
	}
}


//> ../vendor/node-odbc.node
exports = Object.assign( exports, binding.requireBinding() );



getJSBridge().setWriteStreamInitializer(( targetStream: stream.Readable, query: ISqlQueryEx ) =>
{
	targetStream.pipe( new SqlStreamWriter( query ) );
} );

getJSBridge().setReadStreamInitializer(( query: ISqlQueryEx, column: number ) =>
{
	return new SqlStreamReader( query, column );
} );

getJSBridge().setPromiseInitializer(( query: ISqlQueryEx ) => 
{
	return new bluebird(( resolve, reject ) =>
	{
		query.setPromiseInfo( resolve, reject );
	} );
} );
