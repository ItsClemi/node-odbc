import * as odbc from "../lib/node-odbc";
import * as assert from "assert";

import * as mod from "./module";

import * as Benchmark from "benchmark";
import * as async from "async";



let _con: odbc.Connection;

export default {
	name: "test",
	profileIterations: 10,

	setup: function ( )
	{
		_con = new odbc.Connection()
			.connect( mod.connection[0].connectionString );
	},

	exec: function ( cb )
	{
		async.times( 10000, ( n, next ) =>
		{
			_con.executeQuery( odbc.eFetchMode.eArray, ( res, err ) =>
			{
				if( err != null )
				{
					console.log( err );
					throw "LuL";
				}

				for( let i of res )
				{
					console.log( i );
				}

				console.log( n  );

				
				next();
			}, "SELECT * FROM TblTest;" );
		}, cb );
	},

	teardown: ( cb ) =>
	{

	},
};