import * as odbc from "../../lib/node-odbc";
import * as assert from "assert";

import * as mod from "../module";


let _con: odbc.Connection;
let tableName;

export default {
	name: "query-choke",
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


};