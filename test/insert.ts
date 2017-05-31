import * as odbc from "../lib/node-odbc";
import * as assert from "assert";

import module from "./module";


module.connection.forEach(( con ) =>
{
	describe( `Insert test - ${con.name}`, function ()
	{
		//before(( done ) =>
		//{
		//	new odbc.Connection()
		//		.connect( con.connectionString )
		//		.executeQuery( odbc.eFetchMode.eSingle, ( res, err ) =>
		//		{
		//			done( );
		//		},
		//		"drop table tblinsert1;"
		//		);
		//} );

		it( "create table", (done) =>
		{
			new odbc.Connection()
				.connect( con.connectionString )
				.executeQuery( odbc.eFetchMode.eSingle, ( res, err ) =>
				{
					done( err );
				},
				"create table tblinsert1(test int, test2 int, test3 int );"
				);
		} );

		it( "call stored proc (return value)", ( done ) =>
		{
			new odbc.Connection()
				.connect( con.connectionString )
				.executeQuery( odbc.eFetchMode.eSingle, ( res, err ) =>
				{

					done( err );
				},
				""
				);
		} );

	} );
} );

