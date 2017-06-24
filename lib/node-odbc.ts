﻿// File: node-odbc.ts
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
	eBit, eTinyint, eSmallint, eInt32, eUint32, eBigInt, eReal, eChar, eNChar, eVarChar,
	eNVarChar, eBinary, eVarBinary, eDate, eTimestamp, eNumeric,

	eLongVarChar, eLongNVarChar, eLongVarBinary,

	eSqlOutputVar,
}


export class SqlStream 
{
	stream: stream.Readable | stream.Writable;
	length: number;

	constructor( stream: stream.Readable | stream.Writable, length: number )
	{
		this.stream = stream;
		this.length = length;
	}
};

export class SqlNumeric 
{
	precision: number;
	scale: number;
	sign: boolean;
	value: Uint8Array;

	constructor( precision: number, scale: number, sign: boolean, value: Uint8Array )
	{
		this.precision = precision;
		this.scale = scale;
		this.sign = sign;
		this.value = value;
	}
};

export class SqlTimestamp extends Date
{
	nanosecondsDelta: number;
}

export class SqlOutputParameter 
{
	reference: SqlTypes | Uint8Array;
	paramType: eSqlType;
	length?: number;
	precision?: number;
	scale?: number;

	constructor( reference: SqlTypes | Uint8Array, paramType: eSqlType, length?: number, precision?: number, scale?: number )
	{
		this.reference = reference;
		this.paramType = paramType;
		this.length = length;
		this.precision = precision;
		this.scale = scale;
	}
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

	prepareQuery( query: string, typeInfo: Array< eSqlType >, ...args: ( SqlTypes )[] ): ISqlQuery;

	executeQuery<T>( eFetchMode: eFetchMode, cb: ( result: SqlPartialResultTypes<T>, error: SqlError ) => void, query: string, typeInfo: Array<eSqlType>, ...args: ( SqlTypes )[] ): void;

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

		this._connection = new exports.Connection( advancedProps ) as IRawConnection;
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

		let types = this.validateAndPrepareSqlParameters( ...args );

		return this._connection.prepareQuery( query, types, ...args );
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
				throw new Error( `Invalid value range: Expected {${eFetchMode.eSingle} - ${eFetchMode.eArray}} : got ${eFetchOperation}` );
			}
		}

		let types = this.validateAndPrepareSqlParameters( ...args );

		this._connection.executeQuery<T>( eFetchOperation, cb, query, types, ...args );
	}

	public getInfo(): ConnectionInfo
	{
		return this._connection.getInfo();
	}

	private validateAndPrepareSqlParameters( ...args: ( SqlTypes )[] ): Array< eSqlType >
	{
		let types = new Array<eSqlType>( args.length );

		for( var it = 0; it < args.length; it++ )
		{
			types.push( transformParameter( it, args[it] ) );
		}

		return types;
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



export function makeInputStream( stream: fs.ReadStream | stream.Readable, length: number ): SqlStream
{
	return new SqlStream( stream, length );
}

export function makeNumeric( precision: number, scale: number, sign: boolean, value: Uint8Array ): SqlNumeric
{
	return new SqlNumeric( precision, scale, sign, value );
}

export function makeTimestamp( date: Date ): SqlTimestamp
{
	return new SqlTimestamp( date );
}


export const SqlOutput = {

	asBitOutput( reference: boolean ): SqlOutputParameter
	{
		return new SqlOutputParameter( reference, eSqlType.eBit );
	},

	asTinyint( reference: number ): SqlOutputParameter
	{
		return new SqlOutputParameter( reference, eSqlType.eTinyint );
	},

	asSmallint( reference: number ): SqlOutputParameter
	{
		return new SqlOutputParameter( reference, eSqlType.eSmallint );
	},

	asInt( reference: number ): SqlOutputParameter
	{
		return new SqlOutputParameter( reference, eSqlType.eInt32 );
	},

	asUInt( reference: number ): SqlOutputParameter
	{
		return new SqlOutputParameter( reference, eSqlType.eUint32 );
	},

	asBigInt( reference: number ): SqlOutputParameter
	{
		return new SqlOutputParameter( reference, eSqlType.eBigInt );
	},

	asReal( reference: number ): SqlOutputParameter
	{
		return new SqlOutputParameter( reference, eSqlType.eReal );
	},


	asChar( reference: string, length: number ): SqlOutputParameter
	{
		return new SqlOutputParameter( reference, eSqlType.eChar, length );
	},

	asNChar( reference: string, length: number ): SqlOutputParameter
	{
		return new SqlOutputParameter( reference, eSqlType.eNChar, length );
	},

	asVarChar( reference: string, length: number ): SqlOutputParameter
	{
		return new SqlOutputParameter( reference, eSqlType.eVarChar, length );
	},

	asNVarChar( reference: string, length: number ): SqlOutputParameter
	{
		return new SqlOutputParameter( reference, eSqlType.eNVarChar, length );
	},


	asBinary( reference: Uint8Array, length: number ): SqlOutputParameter
	{
		return new SqlOutputParameter( reference, eSqlType.eBinary, length );
	},

	asVarBinary( reference: Uint8Array, length: number ): SqlOutputParameter
	{
		return new SqlOutputParameter( reference, eSqlType.eVarBinary, length );
	},


	asDate( reference: Date, scale: number ): SqlOutputParameter
	{
		return new SqlOutputParameter( reference, eSqlType.eDate, null, null, scale );
	},

	asTimestamp( reference: Date, scale: number ): SqlOutputParameter
	{
		return new SqlOutputParameter( reference, eSqlType.eTimestamp, null, null, scale );
	},


	asNumeric( reference: SqlNumeric, precision: number, scale: number ): SqlOutputParameter
	{
		return new SqlOutputParameter( reference, eSqlType.eNumeric, null, precision, scale );
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


function transformParameter( it: number, i: SqlTypes )
{
	if( i == null )
	{
		return eSqlType.eNull;
	}
	else if( typeof ( i ) == "boolean" )
	{
		return eSqlType.eBit;
	}
	else if( typeof ( i ) == "number" )
	{
		const kMaxInt = 0x7FFFFFFF;
		const kMinInt = -kMaxInt - 1;
		const kMaxUInt32 = 0xFFFFFFFF;

		if( isNaN( i ) || isFinite( i ) )
		{
			throw new Error( `number at ${it} isNan or isFinite ${i}` );
		}

		let even = ( i | 0 ) === i;

		if( even && i !== -0.0 && i >= kMinInt && i <= kMaxInt )
		{
			return eSqlType.eInt32;
		}
		else if( even && i !== -0.0 && i >= 0 && i <= kMaxUInt32 )
		{
			return eSqlType.eUint32;
		}
		else if( even && i !== -0.0 )
		{
			return eSqlType.eBigInt;
		}
		else 
		{
			return eSqlType.eReal;
		}
	}
	else if( typeof ( i ) == "string" )
	{
		return eSqlType.eNVarChar;
	}
	else if( i instanceof Date )
	{
		return eSqlType.eDate;
	}
	else if( i instanceof Buffer )
	{
		return eSqlType.eBinary;
	}
	else if( i instanceof SqlOutputParameter )
	{
		if( enableValidation )
		{

		}

		return eSqlType.eSqlOutputVar;
	}
	else if( i instanceof SqlStream )
	{
		if( enableValidation )
		{
			if( typeof ( i.stream ) != "object" )
			{
				throw new TypeError( `stream: Expected stream, got ${typeof ( stream )}` );
			}

			if( typeof ( i.length ) != "number" )
			{
				throw new TypeError( `length: Expected number, got ${typeof ( length )}` );
			}
		}

		return eSqlType.eVarBinary;
	}
	else if( i instanceof SqlNumeric )
	{
		if( enableValidation )
		{
			if( typeof ( i.precision ) != "number" )
			{
				throw new TypeError( `precision: Expected number, got ${typeof ( i.precision )}` );
			}

			if( typeof ( i.scale ) != "number" )
			{
				throw new TypeError( `scale: Expected number, got ${typeof ( i.scale )}` );
			}

			if( typeof ( i.sign ) != "boolean" )
			{
				throw new TypeError( `sign: Expected boolean, got ${typeof ( i.sign )}` );
			}

			if( typeof ( i.value ) != "object" && !( i.value instanceof Uint8Array ) )
			{
				throw new TypeError( `value: Expected Uint8Array, got ${typeof ( i.value )}` );
			}

			if( i.precision < 0 || i.precision > 255 || i.scale < 0 || i.scale > 127 || i.value.length > 16 )
			{
				throw new Error( `invalid parameter range ${i}` );
			}
		}

		return eSqlType.eNumeric;
	}
	else if( i instanceof SqlTimestamp )
	{
		return eSqlType.eTimestamp;
	}

	throw new Error( `invalid param type ${it} got: ${i}(${typeof ( i )})` );
}