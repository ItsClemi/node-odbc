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
	readonly $sqlQuery?: ISqlQuery;	//transaction
};

export type SqlResult = SqlResultExtension & Partial<any>;

export type SqlPartialResult<T> = SqlResultExtension & Partial<T>;


export type SqlResultArray = SqlResultExtension & Array<any>;

export type SqlPartialResultArray<T> = SqlResultExtension & Array<T>;


export type SqlResultTypes = SqlResult | SqlResultArray;

export type SqlPartialResultTypes<T> = SqlPartialResult<T> & SqlPartialResultArray<T>;

export type SqlTypes = null | string | boolean | number | Date | Buffer | SqlStream | SqlNumeric | SqlTimestamp;


//note: you still have to limit your queries. This is used only for buffer optimization

export const enum eFetchMode {
	eSingle,
	eArray,
};


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
	memoryUsage: number;
	perf: number;
	ioWorker: number;
	odbcConnectionString: string;
	resilienceStrategy: IResilienceStrategy;
};

export type AdvancedConnectionProps = {
	enableMssqlMars?: boolean;
	poolSize?: number;
};

export declare class Connection 
{
	constructor( advancedProps?: AdvancedConnectionProps );

	setResilienceStrategy( strategy: IResilienceStrategy ): Connection;

	//onConnectionFail( cb: ( FailedConnection: con ) => void ): Connection;
	/*
		sets up connection pool
		throws: ISqlError if connect test failed
	*/
	connect( connectionString: string, connectionTimeout?: number ): Connection;

	disconnect( cb: ( ) => void ): void;

	prepareQuery( query: string, ...args: ( SqlTypes )[] ): ISqlQuery;
	
	/*
		callbacks inside execute functions will be called multiple times if chunkSize < resultSet
		this doesn't work with promises because they can't be resolved multiple times
	*/
	executeQuery( eMode: eFetchMode, cb: ( result: SqlResultTypes, error: SqlError ) => void, query: string, ...args: ( SqlTypes )[] ): void;

	executeQuery( eMode: eFetchMode, query: string, ...args: ( SqlTypes )[] ): Promise<SqlResultTypes>;


	getInfo(): ConnectionInfo;
}

export interface ISqlQuery 
{
	addResultSetHandler( eMode: eFetchMode, cb: ( result: SqlResultTypes ) => void ): ISqlQuery;


	enableReturnValue(): ISqlQuery;

	enableMetaData(): ISqlQuery;

	enableSlowQuery(): ISqlQuery;

	enableTransaction (): ISqlQuery;


	setQueryTimeout( timeout: number ): ISqlQuery;

	setChunkSize( chunkSize: number ): ISqlQuery;

	/*
		Runs query and fetch the first row of data
	*/
	toSingle( cb: ( result: SqlResult, error: SqlError ) => void ): void;

	toSingle<T>( cb: ( result: SqlPartialResult<T>, error: SqlError ) => void ): void;


	toSingle(): Promise<SqlResult>;

	toSingle<T>(): Promise<SqlPartialResult<T>>;



	/*
		Runs query and fetch all rows of data
		(fetches only 'chunkSize' number of rows, because promises can't be fired multiple times)
	*/
	toArray( cb: ( result: SqlResultArray, error: SqlError ) => void ): void;

	toArray<T>( cb: ( result: SqlPartialResultArray<T>, error: SqlError ) => void ): void;


	toArray(): Promise<SqlResultArray>;

	toArray<T>(): Promise<SqlPartialResultArray<T>>;



	execute( eMode: eFetchMode, cb: ( result: SqlResultTypes, error: SqlError ) => void ): void;

	execute<T>( eMode: eFetchMode, cb: ( result: SqlPartialResultTypes<T>, error: SqlError ) => void ): void;


	executeRaw( query: string, cb: ( result: SqlResultTypes, error: SqlError ) => void ): void; 

	executeRaw<T>( query: string, cb: ( result: SqlPartialResultTypes<T>, error: SqlError ) => void ): void; 


	rollback( cb: ( err: SqlError ) => void ): void;

	commit( cb: ( err: SqlError ) => void ): void;
}

/*
	c++/js bridge helper
*/
export interface ISqlQueryEx extends ISqlQuery
{
	setPromiseInfo( resolve, reject ): void;
}



//> complex type helper
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
	let writer = new SqlStreamWriter( query );

	targetStream.pipe( writer );
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






///ext

export declare class ConnectionCluster
{
	addConnection( connection: Connection ): ConnectionCluster;
}

//> from https://github.com/aspnet/EntityFramework/blob/f386095005e46ea3aa4d677e4439cdac113dbfb1/src/EFCore.SqlServer/Storage/Internal/SqlServerTransientExceptionDetector.cs
export function getMssqlAzureReconnectStrategy(): IResilienceStrategy
{
	return {
		retries: 5,
		errorCodes: [
			// SQL Error Code: 49920
			// Cannot process request. Too many operations in progress for subscription "%ld".
			// The service is busy processing multiple requests for this subscription.
			// Requests are currently blocked for resource optimization. Query sys.dm_operation_status for operation status.
			// Wait until pending requests are complete or delete one of your pending requests and retry your request later.
			49920,
			// SQL Error Code: 49919
			// Cannot process create or update request. Too many create or update operations in progress for subscription "%ld".
			// The service is busy processing multiple create or update requests for your subscription or server.
			// Requests are currently blocked for resource optimization. Query sys.dm_operation_status for pending operations.
			// Wait till pending create or update requests are complete or delete one of your pending requests and
			// retry your request later.
			49919,
			// SQL Error Code: 49918
			// Cannot process request. Not enough resources to process request.
			// The service is currently busy.Please retry the request later.
			49918,
			// SQL Error Code: 41839
			// Transaction exceeded the maximum number of commit dependencies.
			41839,
			// SQL Error Code: 41325
			// The current transaction failed to commit due to a serializable validation failure.
			41325,
			// SQL Error Code: 41305
			// The current transaction failed to commit due to a repeatable read validation failure.
			41305,
			// SQL Error Code: 41302
			// The current transaction attempted to update a record that has been updated since the transaction started.
			41302,
			// SQL Error Code: 41301
			// Dependency failure: a dependency was taken on another transaction that later failed to commit.
			41301,
			// SQL Error Code: 40613
			// Database XXXX on server YYYY is not currently available. Please retry the connection later.
			// If the problem persists, contact customer support, and provide them the session tracing ID of ZZZZZ.
			40613,
			// SQL Error Code: 40501
			// The service is currently busy. Retry the request after 10 seconds. Code: (reason code to be decoded).
			40501,
			// SQL Error Code: 40197
			// The service has encountered an error processing your request. Please try again.
			40197,
			// SQL Error Code: 10929
			// Resource ID: %d. The %s minimum guarantee is %d, maximum limit is %d and the current usage for the database is %d.
			// However, the server is currently too busy to support requests greater than %d for this database.
			// For more information, see http://go.microsoft.com/fwlink/?LinkId=267637. Otherwise, please try again.
			10929,
			// SQL Error Code: 10928
			// Resource ID: %d. The %s limit for the database is %d and has been reached. For more information,
			// see http://go.microsoft.com/fwlink/?LinkId=267637.
			10928,
			// SQL Error Code: 10060
			// A network-related or instance-specific error occurred while establishing a connection to SQL Server.
			// The server was not found or was not accessible. Verify that the instance name is correct and that SQL Server
			// is configured to allow remote connections. (provider: TCP Provider, error: 0 - A connection attempt failed
			// because the connected party did not properly respond after a period of time, or established connection failed
			// because connected host has failed to respond.)"}
			10060,
			// SQL Error Code: 10054
			// A transport-level error has occurred when sending the request to the server.
			// (provider: TCP Provider, error: 0 - An existing connection was forcibly closed by the remote host.)
			10054,
			// SQL Error Code: 10053
			// A transport-level error has occurred when receiving results from the server.
			// An established connection was aborted by the software in your host machine.
			10053,
			// SQL Error Code: 1205
			// Deadlock
			1205,
			// SQL Error Code: 233
			// The client was unable to establish a connection because of an error during connection initialization process before login.
			// Possible causes include the following: the client tried to connect to an unsupported version of SQL Server;
			// the server was too busy to accept new connections; or there was a resource limitation (insufficient memory or maximum
			// allowed connections) on the server. (provider: TCP Provider, error: 0 - An existing connection was forcibly closed by
			// the remote host.)
			233,
			// SQL Error Code: 121
			// The semaphore timeout period has expired
			121,
			// SQL Error Code: 64
			// A connection was successfully established with the server, but then an error occurred during the login process.
			// (provider: TCP Provider, error: 0 - The specified network name is no longer available.)
			64,
			// DBNETLIB Error Code: 20
			// The instance of SQL Server you attempted to connect to does not support encryption.
			20,
		]
	};
}