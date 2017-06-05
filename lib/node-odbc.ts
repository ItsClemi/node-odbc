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

export type SqlTypes = null | string | boolean | number | Date | Buffer | SqlStream | SqlNumeric | SqlTimestamp;


//> type helpers
const ID_INPUT_STREAM: number = 0;
const ID_NUMERIC_VALUE: number = 1;
const ID_TIMESTAMP_VALUE: number = 2;


export function makeInputStream( stream: fs.ReadStream | stream.Readable, length: number ): SqlStream
{
	return { _typeId: ID_INPUT_STREAM, stream: stream, length: length };
}

export function makeNumericValue( precision: number, scale: number, sign: boolean, value: Uint8Array ): SqlNumeric
{
	return { _typeId: ID_NUMERIC_VALUE, precision: precision, scale: scale, sign: sign, value: value };
}

export function makeTimestampValue( date: Date ): SqlTimestamp
{
	return { _typeId: ID_TIMESTAMP_VALUE, date: date };
}




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

export declare class Connection 
{
	constructor( advancedProps?: ConnectionProps );

	connect( connectionString: string, connectionTimeout?: number ): Connection;

	disconnect( cb: ( ) => void ): void;

	prepareQuery( query: string, ...args: ( SqlTypes )[] ): ISqlQuery;


	//> defaults odbc.eFetchMode.eSingle 
	executeQuery( cb: ( result: SqlResult, error: SqlError ) => void, query: string, ...args: ( SqlTypes )[] ): void;

	executeQuery( eFetchMode: eFetchMode, cb: ( result: SqlResultTypes, error: SqlError ) => void, query: string, ...args: ( SqlTypes )[] ): void;

	executeQuery<T>( cb: ( result: SqlPartialResult<T>, error: SqlError ) => void, query: string, ...args: ( SqlTypes )[] ): void;

	executeQuery<T>( eFetchMode: eFetchMode, cb: ( result: SqlPartialResultTypes<T>, error: SqlError ) => void, query: string, ...args: ( SqlTypes )[] ): void;



	getInfo(): ConnectionInfo;
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


export class ConnectionPool
{
	constructor( props?: ConnectionProps )
	{

		if( props.poolSize != undefined )
		{

		}

	}

	connect( connectionString: string, connectionTimeout?: number)
	{
		let timeout = connectionTimeout || 60000;

	}


}

/*
	c++/js bridge helper
*/
export interface ISqlQueryEx extends ISqlQuery
{
	setPromiseInfo( resolve, reject ): void;
}




//inject node-odbc types in this module scope
exports = Object.assign( exports, require( "../bin/node-odbc.node" ) );



/*
	internal js/C++ bridge helpers

	see docs for more information.
	
	WARNING: you may cause undefined behaviour by changing these
*/

export declare function setWriteStreamInitializer( cb: ( targetStream: stream.Readable, query: ISqlQueryEx ) => void ): void;

export declare function setPromiseInitializer<T>( cb: ( query: ISqlQueryEx ) => T ): void;


export declare function setReadStreamInitializer( cb: ( query: ISqlQueryEx, column: number ) => stream.Readable ): void;

export declare function processNextChunk( query: ISqlQueryEx, chunk: Int8Array, cb: ( error ) => void ): void;
export declare function requestNextChunk( query: ISqlQueryEx, column: number, cb: ( chunk: Int8Array ) => void ): Int8Array;


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
		exports.requestNextChunk( this.query, this.column, ( chunk ) =>
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
		exports.processNextChunk( this._query, chunk, ( error ) =>
		{
			next( error );
		} );
	}
}


exports.setWriteStreamInitializer(( targetStream: stream.Readable, query: ISqlQueryEx ) =>
{
	targetStream.pipe( new SqlStreamWriter( query ) );
} );

exports.setReadStreamInitializer(( query: ISqlQueryEx, column: number ) =>
{
	return new SqlStreamReader( query, column );
} );

exports.setPromiseInitializer(( query: ISqlQueryEx ) => 
{
	return new bluebird(( resolve, reject ) =>
	{
		query.setPromiseInfo( resolve, reject );
	} );
} );



declare class IConnectionPool
{
	enableMssqlMars(): void;



}