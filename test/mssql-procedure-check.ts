import * as assert from "assert";
import * as mod from "./module";
import * as async from "async";

import odbc, { SqlOutput } from "../lib/node-odbc";



mod.connection.forEach(( con ) =>
{
	describe( `procedure checks ${con.name}`, () =>
	{
		let connection: odbc.Connection;
		before(() =>
		{
			connection = new odbc.Connection()
				.connect( con.connectionString );
		} );

		it( "return value", ( done ) =>
		{
		} );

		it( "output (short)", ( done ) =>
		{
			let id = 0;
			let userName: string;
			connection.prepareQuery( "{call uspBasicOutput( ? )}", SqlOutput.asInt( id ) )
				.toSingle()
				.then(( res ) =>
				{

					console.log( id );
				} )
				.catch(( err ) =>
				{
					done( err );
				} );
		} );


	} );
} );
