import * as odbc from "../lib/node-odbc";
import * as assert from "assert";

import module from "./module";


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


module.connection.forEach(( con ) =>
{
	describe( `Connection Tests - ${con.name}`, function ()
	{
		this.timeout( con.timeout );

		afterEach(() =>
		{
			global.gc();
		} );


		it( "setResilienceStrategy - after connect", () =>
		{
			assert.throws(() =>
			{
				new odbc.Connection()
					.connect( con.connectionString )
					.setResilienceStrategy( { retries: 5, errorCodes: [50, 120, 500] } );
			} );
		} );

		it( "setResilienceStrategy - equal", () =>
		{
			let _retries = 25;
			let _errorCodes = [50, 120, 5000, 4000];

			let info: odbc.ConnectionInfo;
			assert.doesNotThrow(() =>
			{
				info = new odbc.Connection()
					.setResilienceStrategy( { retries: _retries, errorCodes: _errorCodes } )
					.connect( con.connectionString )
					.getInfo();
			} );

			assert.ok( info.resilienceStrategy.retries == _retries );
			assert.ok( info.resilienceStrategy.errorCodes.length == _errorCodes.length );

			for( var i = 0; i < _errorCodes.length; i++ )
			{
				assert.ok( _errorCodes[i] === info.resilienceStrategy.errorCodes[i] );
			}
		} );



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
					.executeQuery( <odbc.eFetchMode>( 2500 ), "****select****" );
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
					}, "****select****", <any>{ _typeId: 1, noprecision: true } );
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
					}, "****select****", <any>{ _typeId: 1, precision: true } );
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
					}, "****select****", <any>{ _typeId: 1, precision: 12, scale: false } );
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
					}, "****select****", <any>{ _typeId: 1, precision: 12, scale: 5, sign: "asdf" } );
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
					}, "****select****", <any>{ _typeId: 1, precision: 12, scale: 5, sign: false, value: undefined } );
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
					}, "****select****", <any>{ _typeId: 2, notdate: 25 } );
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
					}, "****select****", <any>{ _typeId: 2, date: false } );
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


	
		//> use pool after dc


		//	assert.throws(() =>
		//	{
		//		_con.executeQuery( odbc.eFetchMode.eSingle, ( res, err ) =>
		//			{
		//				throw "Hello World!";
		//			},
		//			"print 'hello world'"
		//		);
		//	} );

		//} );


		//it( "executeQuery - param binding ", ( done ) =>
		//{
		//	assert.doesNotThrow(() =>
		//	{
		//		new odbc.Connection()
		//			.connect( con.connectionString )
		//			.executeQuery( odbc.eFetchMode.eSingle, ( res, err ) =>
		//			{	
		//				done( err );

		//			}, ";", "asdf", 235, 32.0 );
		//	} );
		//} );



		//it( "executeQuery - memory check", () =>
		//{

		//	assert.doesNotThrow(() =>
		//	{
		//		let _con = new odbc.Connection().connect( con.connectionString );

		//		let count = 0;
		//		let gcount = 0;
		//		let bench = new Benchmark();

		//		let t0 = bench.elapsed();
		//		for( var i = 0; i < 2500000/*Number.MAX_SAFE_INTEGER*/; i++ )
		//		{
		//			_con.executeQuery( odbc.eFetchMode.eSingle, ( res, err ) =>
		//			{

		//			}, "SELECT * TRUE;", "asdf", 235, 32.0 );


		//			count++;

		//			let t1 = bench.elapsed();
		//			if( t0 + 1000 <= t1 )
		//			{
		//				t0 = t1 + 1000;
		//				console.log( `${count} p/sec (${gcount})` );
		//				gcount += count;
		//				count = 0;
		//			}
		//		}
		//	} );

		//} );



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
			assert.ok( typeof ( info.memoryUsage ) === "number" );
			assert.ok( typeof ( info.perf ) === "number" );

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

			//console.log( `${info.dbmsName}: ${info.databaseName}, ${info.driverName}, ${info.driverVersion}, ${info.internalServerType}, ${info.memoryUsage}, ${info.odbcVersion}, ${info.perf}` );
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



		//it( "failed connect - sync", (done) =>
		//{
		//	let con;
		//	assert.doesNotThrow(() =>
		//	{
		//		con = new odbc.Connection( "invalid conection string" )
		//	} );

		//	try
		//	{
		//		con.connect();
		//	}
		//	catch( e )
		//	{
		//		//> isql error

		//	}
		//} );

		//it( "basic execute", (done) =>
		//{
		//	assert.doesNotThrow(() =>
		//	{
		//		new odbc.Connection( dbconf.MssqlConnection, 25 )
		//			.connectAsync(( con, err ) =>
		//			{
		//				if( err != null )
		//				{
		//					done( err );
		//				}

		//				con.executeQuery( odbc.eFetchMode.eVoid, ( res, err ) =>
		//				{
		//					if( err != null )
		//					{
		//						done( err );
		//					}

		//					done();

		//				}, "SELECT * FROM Tbl1" );
		//			} );

		//	} );

		//} );


		//it( "pooled execute", () =>
		//{
		//	assert.doesNotThrow(() =>
		//	{
		//		new odbc.Connection( dbconf.MssqlConnection )
		//			.enableConnectionPool()
		//			.executeQuery( odbc.eFetchMode.eSingle, () => { }, "" );

		//	} );

		//} );


//let con: odbc.Connection;

//declare interface IUser
//{
//	UserName: string;
//	Passsword: string;
//}

//async function getUser( userName : string ) : Promise<any>
//{
//	return await con.prepareQuery( "?={GET_USER( ? )}", userName )
//		.enableReturnValue()
//		.toSingle();
//}

//getUser( "itsclemi" )
//	.then(( user: IUser ) =>
//	{
//	} );



//describe( "Mssql Connection Tests", () =>
//{
//	it( "Mars Execution", (done) =>
//	{
//		assert.doesNotThrow(() =>
//		{
//			new odbc.Connection( dbconf.MssqlConnection )
//				.enableMssqlMarsSupport()
//				.connectAsync(( con, err ) =>
//				{
//					if( err != null ) 
//					{
//						done( err );
//					}


//					con.executeQuery( odbc.eFetchMode.eSingle, () =>
//					{

//					}, "" );
//				} );
//		} );
//	} );


//} );


//describe( "", () =>
//{
//	it( "", () =>
//	{
//		let query = new odbc.Connection( dbconf.MssqlConnection )
//			.enableConnectionPool()
//			.prepareQuery( "asdasd", null )
//			.asCached();






//	} );

//} ); 


//
/*

		assert.throws(() =>
		{
			const con = odbc.Connection( "connection string" )
				.enableConnectionPool( 128 );

			for( var i = 0;i < 25;i++ )
			{
				con.prepareQuery( 'asdasd' )
					.toSingle()
					.then(( data ) =>
					{

					} );
			}

		} );

		assert.throws(() =>
		{
			odbc.Connection( "asdasd" )
				.executeQuery( odbc.eFetchMode.eSingle, ( res, err ) => { }, "aasdasd" );

		} );
*/

/*
	it( "transaction", () =>
	{
		new odbc.Connection( dbconf.MysqlConnection )
			.setConnectionResilienceStrategy( odbc.getMssqlAzureResilienceStrategy() )
			.disableMssqlMarsSupport()
			.connect()
			.prepareQuery( "SELECT * FROM asdf_tbl" )
			.asTransaction()
			.execute( odbc.eFetchMode.eArray, ( res: odbc.SqlResultArray, err ) =>
			{


				if( err != null )
				{
					res.query.rollback(( err ) =>
					{
						if( err != null )
						{
							throw "error";
						}
					} );
				}

				for( var i of res )
				{
					console.log( i.AccountName, i.Password );
				}


				res.query.commit(( err ) =>
				{
					if( err != null )
					{
						throw "failed to update";
					}

					console.log( "success" );
				} );
			} );

	} );

	it( "", () =>
	{
		let con: odbc.Connection;


		con.prepareQuery( "" )
			.toSingle()
			.then(( res ) =>
			{



			} );

		con.prepareQuery( "" )
			.toSingle<IUser>()
			.then(( res ) =>
			{
			} );

		con.prepareQuery( "" )
			.toArray()
			.then(( res ) =>
			{

			} );

		con.prepareQuery( "" )
			.toArray<IUser>()
			.then(( res ) =>
			{
				for( var i of res )
				{
					i.UserName;
				}
			} );

		con.prepareQuery( "" )
			.execute<IUser>( odbc.eFetchMode.eSingle, ( res, err ) =>
			{
				console.log( res.UserName );

			} );

	} );
*/