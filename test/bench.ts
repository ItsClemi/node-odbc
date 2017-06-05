import * as Benchmark from "benchmark";
import * as async from "async";


var suite = new Benchmark.Suite;

export function runBenchmark( test )
{
	var memStart, memMax = memStart = process.memoryUsage().rss;

	suite.add( test.default.name, {
		defer: true,

		fn: function ( deferred )
		{
			test.default.setup((err) =>
			{
				if( err != null )
				{
					console.log( err );
					deferred.resolve();
					return;
				}

				const start = process.hrtime();
				test.default.exec( function ( err )
				{
					const end = process.hrtime( start );
					const elapsed = Math.round(( end[0] * 1000 ) + ( end[1] / 1000000 ) );
					console.log( `> elapsed ${elapsed}` );

					if( err ) throw err;

					memMax = Math.max( memMax, process.memoryUsage().rss );

					deferred.resolve();
				} );
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