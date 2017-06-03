import * as odbc from "../lib/node-odbc";
import * as assert from "assert";

import * as mod from "./module";


mod.connection.forEach(( con ) =>
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

		//it( "create table", (done) =>
		//{
		//	new odbc.Connection()
		//		.connect( con.connectionString )
		//		.executeQuery( odbc.eFetchMode.eSingle, ( res, err ) =>
		//		{
		//			done( err );
		//		},
		//		"create table tblinsert1(test int, test2 int, test3 int );"
		//		);
		//} );

		//it( "call stored proc (return value)", ( done ) =>
		//{
		//	new odbc.Connection()
		//		.connect( con.connectionString )
		//		.executeQuery( odbc.eFetchMode.eSingle, ( res, err ) =>
		//		{

		//			done( err );
		//		},
		//		""
		//		);
		//} );


		//it( "select", ( done ) =>
		//{
		//	new odbc.Connection()
		//		.connect( con.connectionString )
		//		.executeQuery( odbc.eFetchMode.eArray, ( res, err ) =>
		//		{
		//			for( var i of res )
		//			{
		//				//console.log( i );
		//				console.log( `${i.userName} ${i.password} ${i.nn} ${i.asdf} ${i.asdaddf}` );
		//			}

		//		}, "SELECT * FROM TblTest;" );

		//} );

		it( "select bench", () =>
		{
			let _con: odbc.Connection;
			assert.doesNotThrow(() =>
			{
				_con = new odbc.Connection()
					.connect( con.connectionString )

			} );


			assert.doesNotThrow(() =>
			{
				let t = () =>
				{
					_con.executeQuery( odbc.eFetchMode.eArray, ( res, err ) =>
					{
						for( var i of res )
						{
							//console.log( i );
							//console.log( `${i.userName} ${i.password} ${i.nn} ${i.asdf} ${i.asdaddf}` );
						}

					}, "SELECT * FROM TblTest;" );

				};

				for( var i = 0; i < 40000; i++ )
				{
					t();
				}



			} );


		} );

	} );
} );

