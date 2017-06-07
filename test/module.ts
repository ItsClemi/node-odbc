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

export function getRandomTableName( length?: number)
{
	let len = length || 7;

    return "tbl" + getRandomString( len );
}

export function getRandomString(length: number) {
    let possible = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    let text = "";
    for (var i = 0; i < length; i++) {
        text += possible.charAt(Math.floor(Math.random() * possible.length));
    }

    return text;
}


export function getRandomInt( scale: number, min: number )
{
	return Math.floor( ( Math.random() * scale ) + min );
}