import * as Benchmark from "benchmark";
import * as async from "async";


//export function runBenchmark( test )
//{
//	process.nextTick( function ()
//	{
//		var memStart, memMax = memStart = process.memoryUsage().rss;

//		test.default.setup( function ( err )
//		{
//			if( err ) throw err;

//			var n = 0;
//			var bm = new Benchmark( test.default.name, {
//				defer: false,
//				fn: function ( deferred )
//				{

//					test.default.exec( function ( err )
//					{
//						if( err ) throw err;

//						n++;
//						memMax = Math.max( memMax, process.memoryUsage().rss );

//						deferred.resolve();
//					} );
//				}
//			} );

//			bm.on( "complete", function ( event )
//			{
//				console.log( n );
//				console.log( String( event.target ) )
//				console.log( "Memory:", ( memMax - memStart ) / 1024 / 1024, "MiB" )

//				test.default.teardown( function ( err )
//				{
//					if( err ) throw err;
//				} );
//			} );

//			bm.run( { async: false } );
//		} );

//	} );
//}

var suite = new Benchmark.Suite;

export function runBenchmark( test )
{
	var memStart, memMax = memStart = process.memoryUsage().rss;

	suite.add( test.default.name, {
		defer: true,

		setup: () =>
		{
			test.default.setup();
		},
		

		fn: function ( deferred )
		{
			test.default.exec( function ( err )
			{
				if( err ) throw err;

				memMax = Math.max( memMax, process.memoryUsage().rss );

				deferred.resolve();
			} );
		},

		teardown: function ()
		{
			test.default.teardown();
		}

	} ).on( "complete", function ( event )
	{
		console.log( String( event.target ) )
		console.log( "Memory:", ( memMax - memStart ) / 1024 / 1024, "MiB" )

		test.default.teardown( function ( err )
		{
			if( err ) throw err;
		} );
	} ).run();

}