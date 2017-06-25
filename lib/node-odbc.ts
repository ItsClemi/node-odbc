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



export const enum eSqlType
{
	eNull,
	eBit, eTinyint, eSmallint, eInt32, eBigInt, eReal, eChar, eNChar, eVarChar,
	eNVarChar, eBinary, eVarBinary, eDate, eTimestamp, eNumeric,

	eLongVarChar, eLongNVarChar, eLongVarBinary,

	eSqlOutputVar,
}


const ID_INPUT_STREAM: number = 0;
const ID_NUMERIC_VALUE: number = 1;
const ID_TIMESTAMP_VALUE: number = 2;
const ID_OUTPUT_PARAMETER: number = 3;

export type SqlComplexType = {
	readonly _typeId: number;
};

export type SqlStream = SqlComplexType & {
	type: eSqlType;
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
	reference: SqlTypes | Uint8Array;
	paramType: eSqlType;
	length: number;
	precision: number;
	scale: number;
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



export var enableValidation = true;


declare interface IRawConnection
{
	connect( connectionString: string, connectionTimeout?: number ): void;

	disconnect( cb: () => void ): void;

	prepareQuery( query: string, ...args: ( SqlTypes )[] ): ISqlQuery;

	executeQuery<T>( eFetchMode: eFetchMode, cb: ( result: SqlPartialResultTypes<T>, error: SqlError ) => void, query: string, ...args: ( SqlTypes )[] ): void;

	getInfo(): ConnectionInfo;
}

export class Connection
{
	private _connection: IRawConnection;

	constructor( advancedProps?: ConnectionProps )
	{
		if( enableValidation )
		{
			if( advancedProps )
			{
				if( typeof ( advancedProps ) != "object" )
				{
					throw new TypeError( `advancedProps: Expected object, got ${typeof ( advancedProps )}` );
				}

				if( advancedProps.enableMssqlMars != undefined && typeof ( advancedProps.enableMssqlMars ) != "boolean" )
				{
					throw new TypeError( `enableMssqlMars: Expected boolean, got ${typeof ( advancedProps.enableMssqlMars )}` );
				}

				if( advancedProps.poolSize != undefined && typeof ( advancedProps.poolSize ) != "number" )
				{
					throw new TypeError( `poolSize: Expected number, got ${typeof ( advancedProps.poolSize )}` );
				}
			}
		}

		this._connection = new exports.IRawConnection( advancedProps ) as IRawConnection;
	}

	public connect( connectionString: string, connectionTimeout?: number ): Connection
	{
		if( enableValidation )
		{
			if( typeof ( connectionString ) != "string" )
			{
				throw new TypeError( `connectionString: Expected string, got ${typeof ( connectionString )}` );
			}

			if( connectionTimeout != undefined && typeof ( connectionTimeout ) != "number" )
			{
				throw new TypeError( `connectionTimeout: Expected number, got ${typeof ( connectionTimeout )}` );
			}
		}

		this._connection.connect( connectionString, connectionTimeout );

		return this;
	}

	public disconnect( cb: () => void ): void
	{
		if( enableValidation )
		{
			if( typeof ( cb ) != "function" )
			{
				throw new TypeError( `cb: Expected function, got ${typeof ( cb )}` );
			}
		}

		this._connection.disconnect( cb );
	}

	public prepareQuery( query: string, ...args: ( SqlTypes )[] ): ISqlQuery
	{
		if( enableValidation )
		{
			if( typeof ( query ) != "string" )
			{
				throw new TypeError( `query: Expected string, got ${typeof ( query )}` );
			}
		}

		return this._connection.prepareQuery( query, ...args );
	}

	public executeQuery<T>( eFetchOperation: eFetchMode, cb: ( result: SqlPartialResultTypes<T>, error: SqlError ) => void, query: string, ...args: ( SqlTypes )[] ): void
	{
		if( enableValidation ) 
		{
			if( typeof ( eFetchOperation ) != "number" )
			{
				throw new TypeError( `eFetchOperation: Expected number, got ${typeof ( cb )}` );
			}

			if( typeof ( cb ) != "function" )
			{
				throw new TypeError( `cb: Expected function, got ${typeof ( cb )}` );
			}

			if( typeof ( query ) != "string" )
			{
				throw new TypeError( `query: Expected string, got ${typeof ( query )}` );
			}

			if( eFetchOperation < 0 || eFetchOperation > eFetchMode.eArray )
			{
				throw new Error( `eFetchOperation: Invalid value range: Expected {${eFetchMode.eSingle} - ${eFetchMode.eArray}} : got ${eFetchOperation}` );
			}
		}

		this._connection.executeQuery<T>( eFetchOperation, cb, query, ...args );
	}

	public getInfo(): ConnectionInfo
	{
		return this._connection.getInfo();
	}
}

export interface ISqlQuery 
{
	enableReturnValue(): ISqlQuery;

	enableMetaData(): ISqlQuery;

	setQueryTimeout( timeout: number ): ISqlQuery;


	toSingle<T>( cb: ( result: SqlPartialResult<T>, error: SqlError ) => void ): void;

	toSingle<T>(): Promise<SqlPartialResult<T>>;

	toArray<T>( cb: ( result: SqlPartialResultArray<T>, error: SqlError ) => void ): void;

	toArray<T>(): Promise<SqlPartialResultArray<T>>;
}

export interface ISqlQueryEx extends ISqlQuery
{
	setPromiseInfo( resolve, reject ): void;
}



export function makeInputStream( type: eSqlType.eLongNVarChar | eSqlType.eLongVarBinary, stream: fs.ReadStream | stream.Readable, length: number ): SqlStream
{
	return { _typeId: ID_INPUT_STREAM, type, stream, length };
}

export function makeNumeric( precision: number, scale: number, sign: boolean, value: Uint8Array ): SqlNumeric
{
	return { _typeId: ID_NUMERIC_VALUE, precision, scale, sign, value };
}

export function makeTimestamp( date: Date ): SqlTimestamp
{
	return { _typeId: ID_TIMESTAMP_VALUE, date };
}

function makeOutputParameter( reference: SqlTypes | Uint8Array, paramType: eSqlType, length?: number, precision?: number, scale?: number ): SqlOutputParameter
{
	return { _typeId: ID_OUTPUT_PARAMETER, reference, paramType, length: length || 0, precision: precision || 0, scale: scale || 0 };
}

export const SqlOutput = {

	asBitOutput( reference: boolean ): SqlOutputParameter
	{
		return makeOutputParameter( reference, eSqlType.eBit );
	},

	asTinyint( reference: number ): SqlOutputParameter
	{
		return makeOutputParameter( reference, eSqlType.eTinyint );
	},

	asSmallint( reference: number ): SqlOutputParameter
	{
		return makeOutputParameter( reference, eSqlType.eSmallint );
	},

	asInt( reference: number ): SqlOutputParameter
	{
		return makeOutputParameter( reference, eSqlType.eInt32 );
	},

	asBigInt( reference: number ): SqlOutputParameter
	{
		return makeOutputParameter( reference, eSqlType.eBigInt );
	},

	asReal( reference: number ): SqlOutputParameter
	{
		return makeOutputParameter( reference, eSqlType.eReal );
	},

	asChar( reference: string, length: number ): SqlOutputParameter
	{
		return makeOutputParameter( reference, eSqlType.eChar, length );
	},

	asNChar( reference: string, length: number ): SqlOutputParameter
	{
		return makeOutputParameter( reference, eSqlType.eNChar, length );
	},

	asVarChar( reference: string, length: number ): SqlOutputParameter
	{
		return makeOutputParameter( reference, eSqlType.eVarChar, length );
	},

	asNVarChar( reference: string, length: number ): SqlOutputParameter
	{
		return makeOutputParameter( reference, eSqlType.eNVarChar, length );
	},

	asBinary( reference: Uint8Array, length: number ): SqlOutputParameter
	{
		return makeOutputParameter( reference, eSqlType.eBinary, length );
	},

	asVarBinary( reference: Uint8Array, length: number ): SqlOutputParameter
	{
		return makeOutputParameter( reference, eSqlType.eVarBinary, length );
	},

	asDate( reference: Date, scale: number ): SqlOutputParameter
	{
		return makeOutputParameter( reference, eSqlType.eDate, undefined, undefined, scale );
	},

	asTimestamp( reference: Date, scale: number ): SqlOutputParameter
	{
		return makeOutputParameter( reference, eSqlType.eTimestamp, undefined, undefined, scale );
	},

	asNumeric( reference: SqlNumeric, precision: number, scale: number ): SqlOutputParameter
	{
		return makeOutputParameter( reference, eSqlType.eNumeric, undefined, precision, scale );
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

