import * as odbc from "../lib/node-odbc";
import * as assert from "assert";

import * as mod from "./module";


declare interface IUser
{
	UserName: string;
	Password: string;
}

class Benchmark
{

	private start = process.hrtime();

	public elapsed(): number
	{
		const end = process.hrtime( this.start );
		return Math.round(( end[0] * 1000 ) + ( end[1] / 1000000 ) );
	}
}


mod.connection.forEach(( con ) =>
{
	describe( `Connection Tests - ${con.name}`, function ()
	{
		this.timeout( con.timeout );



		//it( "setResilienceStrategy - after connect", () =>
		//{
		//	assert.throws(() =>
		//	{
		//		new odbc.Connection()
		//			.connect( con.connectionString )
		//			.setResilienceStrategy( { retries: 5, errorCodes: [50, 120, 500] } );
		//	} );
		//} );

		//it( "setResilienceStrategy - equal", () =>
		//{
		//	let _retries = 25;
		//	let _errorCodes = [50, 120, 5000, 4000];

		//	let info: odbc.ConnectionInfo = <any>{};
		//	assert.doesNotThrow(() =>
		//	{
		//		info = new odbc.Connection()
		//			.setResilienceStrategy( { retries: _retries, errorCodes: _errorCodes } )
		//			.connect( con.connectionString )
		//			.getInfo();
		//	} );

		//	assert.ok( info.resilienceStrategy.retries == _retries );
		//	assert.ok( info.resilienceStrategy.errorCodes.length == _errorCodes.length );

		//	for( var i = 0; i < _errorCodes.length; i++ )
		//	{
		//		assert.ok( _errorCodes[i] === info.resilienceStrategy.errorCodes[i] );
		//	}
		//} );



		it( "connect - basic connect", () =>
		{
			assert.doesNotThrow(() =>
			{
				new odbc.Connection()
					.connect( con.connectionString );
			} );
		} );

		it( "connect - basic connect(timeout)", () =>
		{
			assert.doesNotThrow(() =>
			{
				new odbc.Connection()
					.connect( con.connectionString, con.timeout );
			} );
		} );


		it( "executeQuery - invalid fetch mode range", () =>
		{
			assert.throws(() =>
			{
				new odbc.Connection()
					.connect( con.connectionString )
					.executeQuery( <odbc.eFetchMode>( 2500 ), () => { }, "****select****" );
			} );
		} );


		it( "executeQuery - invalid complex param (no typeid)", () =>
		{
			assert.throws(() =>
			{
				new odbc.Connection()
					.connect( con.connectionString )
					.executeQuery( odbc.eFetchMode.eSingle, ( res, err ) =>
					{
					}, "****select****", <any>{} );
			} );
		} );

		it( "executeQuery - invalid complex param (undefined)", () =>
		{
			assert.throws(() =>
			{
				new odbc.Connection()
					.connect( con.connectionString )
					.executeQuery( odbc.eFetchMode.eSingle, ( res, err ) =>
					{
					}, "****select****", <any>{ noid: false } );
			} );
		} );

		it( "executeQuery - invalid complex param (invalid typeid type)", () =>
		{
			assert.throws(() =>
			{
				new odbc.Connection()
					.connect( con.connectionString )
					.executeQuery( odbc.eFetchMode.eSingle, ( res, err ) =>
					{
					}, "****select****", <any>{ _typeId: false } );
			} );
		} );


		it( "executeQuery - invalid complex param (stream)", () =>
		{
			assert.throws(() =>
			{
				new odbc.Connection()
					.connect( con.connectionString )
					.executeQuery( odbc.eFetchMode.eSingle, ( res, err ) =>
					{
					}, "****select****", <any>{ _typeId: 0, nostream: 25 } );
			} );
		} );

		it( "executeQuery - invalid complex param (stream invalid type A)", () =>
		{
			assert.throws(() =>
			{
				new odbc.Connection()
					.connect( con.connectionString )
					.executeQuery( odbc.eFetchMode.eSingle, ( res, err ) =>
					{
					}, "****select****", <any>{ _typeId: 0, stream: false } );
			} );
		} );

		it( "executeQuery - invalid complex param (stream invalid type B)", () =>
		{
			assert.throws(() =>
			{
				new odbc.Connection()
					.connect( con.connectionString )
					.executeQuery( odbc.eFetchMode.eSingle, ( res, err ) =>
					{
					}, "****select****", <any>{ _typeId: 0, stream: {}, length: false } );
			} );
		} );


		it( "executeQuery - invalid complex param (numeric)", () =>
		{
			assert.throws(() =>
			{
				new odbc.Connection()
					.connect( con.connectionString )
					.executeQuery( odbc.eFetchMode.eSingle, ( res, err ) =>
					{
					}, "****select****", ( <any>odbc ).makeNumeric( undefined ) );
			} );
		} );

		it( "executeQuery - invalid complex param (numeric invalid type A)", () =>
		{
			assert.throws(() =>
			{
				new odbc.Connection()
					.connect( con.connectionString )
					.executeQuery( odbc.eFetchMode.eSingle, ( res, err ) =>
					{
					}, "****select****", odbc.makeNumeric( true as any, 5, false, <any>undefined ) );
			} );
		} );

		it( "executeQuery - invalid complex param (numeric invalid type B)", () =>
		{
			assert.throws(() =>
			{
				new odbc.Connection()
					.connect( con.connectionString )
					.executeQuery( odbc.eFetchMode.eSingle, ( res, err ) =>
					{
					}, "****select****", odbc.makeNumeric( 12, <any>false, false, <any>undefined ) );
			} );
		} );

		it( "executeQuery - invalid complex param (numeric invalid type C)", () =>
		{
			assert.throws(() =>
			{
				new odbc.Connection()
					.connect( con.connectionString )
					.executeQuery( odbc.eFetchMode.eSingle, ( res, err ) =>
					{
					}, "****select****", odbc.makeNumeric( 12, 5, <any>"asdasds", <any>undefined ) );
			} );
		} );

		it( "executeQuery - invalid complex param (numeric invalid type D)", () =>
		{
			assert.throws(() =>
			{
				new odbc.Connection()
					.connect( con.connectionString )
					.executeQuery( odbc.eFetchMode.eSingle, ( res, err ) =>
					{
					}, "****select****", odbc.makeNumeric( 12, 5, false, <any>undefined ) );
			} );
		} );


		it( "executeQuery - invalid complex param (timestamp)", () =>
		{
			assert.throws(() =>
			{
				new odbc.Connection()
					.connect( con.connectionString )
					.executeQuery( odbc.eFetchMode.eSingle, ( res, err ) =>
					{
					}, "****select****", odbc.makeTimestamp( <any>( { notdate: 25 } ) ) );
			} );
		} );

		it( "executeQuery - invalid complex param (timestamp invalid type)", () =>
		{
			assert.throws(() =>
			{
				new odbc.Connection()
					.connect( con.connectionString )
					.executeQuery( odbc.eFetchMode.eSingle, ( res, err ) =>
					{
					}, "****select****", odbc.makeTimestamp( <any>( undefined ) ) );
			} );
		} );


		it( "executeQuery - invalid sql syntax (no params)", ( done ) =>
		{
			assert.doesNotThrow(() =>
			{
				new odbc.Connection()
					.connect( con.connectionString )
					.executeQuery( odbc.eFetchMode.eSingle, ( res, err ) =>
					{
						if( err == null )
						{
							done( new Error( "err == null" ) );
						}
						else
						{
							done();
						}
					},
					"sel asdf frm tbl001"
				);
			} );
		} );

		it( "executeQuery - invalid sql syntax (string, number)", ( done ) =>
		{
			assert.doesNotThrow(() =>
			{
				new odbc.Connection()
					.connect( con.connectionString )
					.executeQuery( odbc.eFetchMode.eSingle, ( res, err ) =>
					{
						if( err == null )
						{
							done( new Error( "err == null" ) );
						}
						else
						{
							done();
						}
					},
					"sel asdf frm tbl001", "test", 1234
					);
			} );
		} );




		it( "disconnect - double disconnect", () =>
		{
			let _con: odbc.Connection;
			assert.doesNotThrow(() =>
			{
				_con = new odbc.Connection()
					.connect( con.connectionString );

				_con.executeQuery( odbc.eFetchMode.eSingle, ( res, err ) => { }, "WAITFOR DELAY '00:00:01';" );		
				_con.disconnect(() =>
				{

				} );
			} );

			assert.throws(() =>
			{
				_con.disconnect(() =>
				{

				} );
			} );			
		} );

		it( "disconnect", ( done ) =>
		{
			assert.doesNotThrow(() =>
			{
				let _con = new odbc.Connection()
					.connect( con.connectionString );

				_con.executeQuery( odbc.eFetchMode.eSingle, ( res, err ) => { }, "WAITFOR DELAY '00:00:01';" );

				_con.disconnect(() =>
				{
					done();
				} );
			} );
		} );

		it( "disconnect - block setResilienceStrategy", () =>
		{
			let _con: odbc.Connection;

			assert.doesNotThrow(() =>
			{
				_con = new odbc.Connection()
					.connect( con.connectionString );

				_con.executeQuery( odbc.eFetchMode.eSingle, ( res, err ) => { }, "WAITFOR DELAY '00:00:01';" );

				_con.disconnect(() =>
				{
				} );
			} );

			assert.throws(() =>
			{
				_con.connect( con.connectionString );
			} );
		} );

		it( "disconnect - block prepareQuery", () =>
		{
			let _con: odbc.Connection;

			assert.doesNotThrow(() =>
			{
				_con = new odbc.Connection()
					.connect( con.connectionString );

				_con.executeQuery( odbc.eFetchMode.eSingle, ( res, err ) => { }, "WAITFOR DELAY '00:00:01';" );

				_con.disconnect(() =>
				{
				} );
			} );

			assert.throws(() =>
			{
				_con.prepareQuery( "query" );
			} );
		} );

		it( "disconnect - block connect", () =>
		{
			let _con: odbc.Connection;

			assert.doesNotThrow(() =>
			{
				_con = new odbc.Connection()
					.connect( con.connectionString );

				_con.executeQuery( odbc.eFetchMode.eSingle, ( res, err ) => { }, "WAITFOR DELAY '00:00:01';" );

				_con.disconnect(() =>
				{
				} );
			} );

			assert.throws(() =>
			{
				_con.connect( con.connectionString );
			} );
		} );

		it( "disconnect - block executeQuery", () =>
		{
			let _con: odbc.Connection;

			assert.doesNotThrow(() =>
			{
				_con = new odbc.Connection()
					.connect( con.connectionString );

				_con.executeQuery( odbc.eFetchMode.eSingle, ( res, err ) => { }, "WAITFOR DELAY '00:00:01';" );

				_con.disconnect(() =>
				{
				} );
			} );

			assert.throws(() =>
			{
				_con.executeQuery( odbc.eFetchMode.eSingle, ( res, err ) => { }, "SELECT" );
			} );
		} );

		it( "disconnect - block getInfo", () =>
		{
			let _con: odbc.Connection;

			assert.doesNotThrow(() =>
			{
				_con = new odbc.Connection()
					.connect( con.connectionString );

				_con.executeQuery( odbc.eFetchMode.eSingle, ( res, err ) => { }, "WAITFOR DELAY '00:00:01';" );

				_con.disconnect(() =>
				{
				} );
			} );

			assert.throws(() =>
			{
				_con.getInfo( );
			} );
		} );



		it( "getInfo - basic", () =>
		{
			let info: odbc.ConnectionInfo = <any>{};
			assert.doesNotThrow(() =>
			{
				info = new odbc.Connection()
					.connect( con.connectionString )
					.getInfo();
			} );

			assert.ok( typeof ( info.databaseName ) === "string" );
			assert.ok( typeof ( info.driverName ) === "string" );
			assert.ok( typeof ( info.driverVersion ) === "string" );
			assert.ok( typeof ( info.dbmsName ) === "string" );
			assert.ok( typeof ( info.odbcVersion ) === "string" );
			assert.ok( typeof ( info.odbcConnectionString ) === "string" );


			assert.ok( typeof ( info.internalServerType ) === "number" );
			assert.ok( info.databaseName.length > 0 );
			assert.ok( info.driverName.length > 0 );
			assert.ok( info.driverVersion.length > 0 );
			assert.ok( info.odbcVersion.length > 0 );
			assert.ok( info.dbmsName.length > 0 );
			assert.ok( info.odbcConnectionString.length > 0 );

			let arr = [
				info.databaseName,
				info.driverName,
				info.driverVersion,
				info.dbmsName,
				info.odbcVersion,
				info.odbcConnectionString,
			];

			for( var x of arr )
			{
				let it = 0;
				for( var y of arr )
				{
					if( x === y )
					{
						it++;
					}
				}

				assert.ok( it == 1, "string value duplicated" );
			}
		} );

		it( "getInfo - detect server type", () =>
		{
			let info: odbc.ConnectionInfo = <any>{};
			assert.doesNotThrow(() =>
			{
				info = new odbc.Connection()
					.connect( con.connectionString )
					.getInfo();
			} );

			let server = [
				{ type: 1, dbms: "Microsoft SQL Server" },
				{ type: 2, dbms: "MySQL" },
			];


			if( info.internalServerType == 0 )
			{
				for( var i of server )
				{
					assert.ok( i.dbms != info.dbmsName );
				}
			}
			else
			{
				for( var i of server )
				{
					if( info.internalServerType == i.type )
					{
						assert.ok( info.dbmsName == i.dbms, `invalid type mapping ${info.dbmsName} to ${i.dbms}` );
						return;
					}
				}

				assert.ok( false, `${info.internalServerType} invalid mapping to: ${info.dbmsName}` );
			}
		} );

	} );
} );
