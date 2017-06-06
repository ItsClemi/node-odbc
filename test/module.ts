import * as odbc from "../lib/node-odbc";
import * as assert from "assert";
import * as stream from "stream";

var dbconf;
if( process.env.NODE_ODBC_TEST_USE_APPVEYOR_CONFIG )
{
	dbconf = require( "./dbconf.appveyor.json" );
}
else
{
	dbconf = require( "./dbconf.json" );
}


export declare type ConnectionEntry = {
	name: string;
	connectionString: string;
	timeout: number;
}; 

export var connection = <Array<ConnectionEntry>>dbconf.Connections;

export class SqlStreamReader extends stream.Readable
{
	private query: odbc.ISqlQueryEx;
	private column: number;

	constructor( _query: odbc.ISqlQueryEx, _column: number )
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

export class SqlStreamWriter extends stream.Writable
{
	private _query: odbc.ISqlQueryEx;

	constructor( query: odbc.ISqlQueryEx )
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

