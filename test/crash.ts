import * as odbc from "../lib/node-odbc";
import * as assert from "assert";

import * as module from "./module";

describe( "crash", () =>
{

	//-> not fixed, due to refcounting from v8
	it( "executeQuery - throw in callback", ( done ) =>
	{
		assert.doesNotThrow(() =>
		{
			new odbc.Connection()
				.connect( module.connection[0].connectionString )
				.executeQuery( odbc.eFetchMode.eSingle, ( res, err ) =>
				{
					throw "lul";
				},
				"sel asdf frm tbl001"
				);

		} );
	} );

	//-> fixed
	it( "executeQuery - throw in callback", () =>
	{
		assert.throws(() =>
		{
			new odbc.Connection()
				.connect( module.connection[0].connectionString )
				.executeQuery( odbc.eFetchMode.eSingle, ( res, err ) =>
				{
					throw "lul";
				},
				"sel asdf frm tbl001"
				);

		} );
	} );

} );