import * as odbc from "../lib/node-odbc";
import * as assert from "assert";

import * as mod from "./module";

import * as Benchmark from "benchmark";
import * as async from "async";



let _con: odbc.Connection;
let tableName;

export default {
	name: "test",
	profileIterations: 10,

	setup: function ( done )
	{
		tableName = `tblTestInsert${Math.floor( Math.random() * 100 )}`

		_con = new odbc.Connection()
			.connect( mod.connection[0].connectionString );


		_con.executeQuery(
			odbc.eFetchMode.eSingle,
			( res, err ) =>
			{
				if( err )
				{
					console.log( err );
					throw err;
				}

				console.log( `tbl created ${tableName}` );

				done();
			},
			`CREATE TABLE ${tableName}(a varchar( 256 ) NOT NULL, b nvarchar( 128 ) NOT NULL, c int NOT NULL, d bigint NOT NULL, e real NOT NULL)`
		);
	},



	exec: function ( done )
	{
		async.times( 10000, ( n, next ) =>
		{
			_con.executeQuery(
				( res, err ) =>
				{
					if( err ) 
					{
						console.log( err );
					}

					next();
				},

				`INSERT INTO ${tableName}(a, b, c, d, e )VALUES(?, ?, ?, ?, ?)`,

				"Hello World!",
				"Unicode: \u01F8\u01EA\u01F7\u01B9\u007E\u00F4\u0110\u0243\u0187",
				Math.floor( Math.random() * 250 ),
				Math.floor(( Math.random() * 4294967296 ) + 4294967296 ),
				Math.random() * 100
			);


			_con.executeQuery(( res, err ) =>
			{

			}, `INSERT INTO ${tableName}(a, b) VALUES(?,?)`, "asdasd", "YOOOOO" );
		}, done );
	},

	teardown: () =>
	{
		_con.executeQuery(
			( res, err ) =>
			{
				if( err != null )
				{
					throw err;
				}
			},
			`DROP TABLE ${tableName}`
		);
	},
};