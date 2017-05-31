import * as assert from "assert";
import * as odbc from "../lib/node-odbc";


describe( "api tests - connection", () =>
{
	afterEach(() =>
	{
		global.gc();
	} );


	it( "constructor: non new cast", () =>
	{
		assert.doesNotThrow(() =>
		{
			( <any>odbc ).Connection();
		} );
	} );

	it( "constructor: new cast", () =>
	{
		assert.doesNotThrow(() =>
		{
			new odbc.Connection();
		} );
	} );

	it( "constructor: too many arguments", () =>
	{
		assert.throws(() =>
		{
			new ( <any>odbc ).Connection( 25, true, false, null, undefined );
		} );
	} );

	it( "constructor: invalid parameter", () =>
	{
		assert.throws(() =>
		{
			new odbc.Connection( 2552 );
		} );
	} );

	it( "constructor: valid connection props", () =>
	{
		assert.doesNotThrow(() =>
		{
			new odbc.Connection( { enableMssqlMars: true, poolSize: 128 } );
		} );
	} );

	it( "constructor: invalid connection props", () =>
	{
		assert.doesNotThrow(() =>
		{
			new odbc.Connection( <any>{ noenable: false } );
		} );
	} );

	it( "constructor: invalid connection props(empty)", () =>
	{
		assert.doesNotThrow(() =>
		{
			new odbc.Connection( <any>{} );
		} );
	} );

	it( "constructor: invalid connection props (invalid param A)", () =>
	{
		assert.throws(() =>
		{
			new odbc.Connection( <any>{ enableMssqlMars: "asdsad" } );
		} );
	} );

	it( "constructor: invalid connection props (invalid param B)", () =>
	{
		assert.throws(() =>
		{
			new odbc.Connection( <any>{ enableMssqlMars: true, poolSize: true } );
		} );
	} );


	it( "setResilienceStrategy - invalid parameter", () =>
	{
		assert.throws(() =>
		{
			new odbc.Connection()
				.setResilienceStrategy( undefined );
		} );
	} );

	it( "setResilienceStrategy - no parameter", () =>
	{
		assert.throws(() =>
		{
			new odbc.Connection()
				.setResilienceStrategy( <any>{ noretries: false } );
		} );
	} );

	it( "setResilienceStrategy - valid parameter A", () =>
	{
		assert.throws(() =>
		{
			new odbc.Connection()
				.setResilienceStrategy( <any>{ retries: 25, errorCodes: undefined } );
		} );
	} );

	it( "setResilienceStrategy - valid parameter B", () =>
	{
		assert.throws(() =>
		{
			new odbc.Connection()
				.setResilienceStrategy( <any>{ retries: false, errorCodes: [25, 30] } );
		} );
	} );

	it( "setResilienceStrategy - valid parameters", () =>
	{
		assert.doesNotThrow(() =>
		{
			new odbc.Connection()
				.setResilienceStrategy( { retries: 5, errorCodes: [128, 40005, 23] } );
		} );
	} );

	it( "setResilienceStrategy - invalid array parameters", () =>
	{
		assert.throws(() =>
		{
			new odbc.Connection()
				.setResilienceStrategy( <any>{ retries: 5, errorCodes: [128, undefined, 40005, "asdasd", 23] } );
		} );
	} );


	it( "connect - too many arguments", () =>
	{
		assert.throws(() =>
		{
			new ( <any>odbc ).Connection()
				.connect( null, 123123, false, true, undefined );
		} );
	} );

	it( "connect - wrong parameter type A", () =>
	{
		assert.throws(() =>
		{
			new ( <any>odbc ).Connection()
				.connect( false );
		} );
	} );

	it( "connect - wrong parameter type B", () =>
	{
		assert.throws(() =>
		{
			new ( <any>odbc ).Connection()
				.connect( "test", false );
		} );
	} );

	it( "connect - double connect", () =>
	{
		assert.throws(() =>
		{
			new odbc.Connection()
				.connect( "test", 25 )
				.connect( "test1", 45 );
		} );
	} );

	it( "connect - valid parameters (invalid connectionString)", ( done ) =>
	{
		try
		{
			new odbc.Connection()
				.connect( "HelloWorld", 15 );
		}
		catch( err )
		{
			if( typeof ( err.message ) === "string" &&
				typeof ( err.sqlState ) === "string" &&
				typeof ( err.code ) === "number" )
			{
				if( err.sqlState === "IM002" )
				{
					done();
					return;
				}
			}

			console.log( `${err.message}, ${err.sqlState}, ${err.code}` );

			done( err );
		}
	} );


	it( "disconnect - invalid pool state", () =>
	{
		assert.throws(() =>
		{
			new odbc.Connection()
				.disconnect(() => { } );
		} );
	} );

	it( "disconnect - invalid parameter", () =>
	{
		assert.throws(() =>
		{
			new odbc.Connection()
				.disconnect( null );
		} );
	} );

	it( "disconnect - too many arguments", () =>
	{
		assert.throws(() =>
		{
			( <any>new odbc.Connection() )
				.disconnect( true, false, undefined );
		} );
	} );


	it( "prepareQuery - not connected", () =>
	{
		assert.throws(() =>
		{
			new odbc.Connection()
				.prepareQuery( "asdf" );
		} );
	} );

	it( "prepareQuery - invalid parameter A", () =>
	{
		assert.throws(() =>
		{
			new odbc.Connection()
				.prepareQuery( undefined );
		} );
	} );


	it( "executeQuery - invalid fetch mode", () =>
	{
		assert.throws(() =>
		{
			new odbc.Connection()
				.executeQuery( <odbc.eFetchMode>( <any>"hello world" ), "" );
		} );
	} );

	it( "executeQuery - invalid parameter 2", () =>
	{
		assert.throws(() =>
		{
			new odbc.Connection()
				.executeQuery( odbc.eFetchMode.eSingle, <string>( <any>undefined ) );
		} );
	} );

	it( "executeQuery - not connected", () =>
	{
		assert.throws(() =>
		{
			new odbc.Connection()
				.executeQuery( odbc.eFetchMode.eSingle, "" );
		} );
	} );


	it( "getInfo - not connected", () =>
	{
		assert.throws(() =>
		{
			new odbc.Connection()
				.getInfo();
		} );
	} );
} );

