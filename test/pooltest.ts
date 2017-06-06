import * as assert from "assert";
import * as odbc from "../dist/node-odbc";

import * as mod from "./module";

import * as os from "os";

let cpus = os.cpus().length;


async function fCreateTable( con: odbc.Connection )
{
	await con.prepareQuery( "CREATE TABLE tblMassInsert( a int, b int, c int )" ).toSingle( );
}


mod.connection.forEach((con) =>
{
	describe( `pool test - ${con.name}`, ( ) =>
	{
		it( 'massive insert', async( done ) =>
		{
			let _con = new odbc.Connection()
				.connect( con.connectionString );

			await fCreateTable( _con );
				
			for( var i = 0; i < cpus * 200; i++ )
			{
				_con.executeQuery( odbc.eFetchMode.eSingle, ( res, err ) =>
				{
					if( err != null )
					{
						done( new Error( "query failed!" ) );
					}
					else
					{
						if( i == ( ( cpus * 200 ) - 1 ) )
						{
							done();
						}
					}

				}, "INSERT INTO tblMassInsert( a, b, c ) VALUES( ?, ?, ? )", Math.floor( Math.random() * 25 ), Math.floor( Math.random() * 30 ), Math.floor( Math.random() * 45 ) );
			}

		} );

	} ); 

} );