import * as assert from "assert";
import * as mod from "./module";
import * as async from "async";

import * as odbc from "../lib/node-odbc";

mod.connection.forEach(( con ) =>
{
	describe( `procedure checks ${con.name}`, () =>
	{
		let connection: odbc.Connection;
		before(() =>
		{
			connection = new odbc.Connection()
				.connect( con.connectionString );
			console.log( connection );
		} );

		it( "output (short)", ( done ) =>
		{
			let procName = mod.getRandomProcedureName( 12 );

			connection.prepareQuery( `CREATE PROCEDURE ${procName} @A INT OUTPUT AS SET @A = 25; RETURN` )
				.toSingle()
				.then(() =>
				{
					let A = 0;
					let B = odbc.SqlOutput.asInt( A );


					connection.prepareQuery( `{call ${procName}( ? )}`, B)
						.toSingle()
						.then(( res ) =>
						{
							console.log( A );
							console.log( B );
						} )
						.catch(( err ) =>
						{
							done( err );
						} );
				} )
				.catch(( err ) =>
				{
					done( err );
				});
		} );


	} );
} );
