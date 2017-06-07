import * as odbc from "../lib/node-odbc";
import * as assert from "assert";
import * as mocha from "mocha";
import * as path from "path";
import * as fs from "fs";
import * as bench from "./bench";



let tests = new mocha();

tests.checkLeaks();

tests.addFile( "./test/api.js" );
tests.addFile( "./test/connection.js" );

tests.addFile( "./test/query-type-check.js" );
tests.addFile( "./test/query-type-check-promise.js" );


tests.run();



function runBenchmark()
{
	console.log( "running benchmark" );

	const insert = require( "./benchmark/basic-insert" );

	bench.runBenchmark( insert );

}


//runBenchmark();



//> don't close console window
let inv = setInterval(() => { }, 1000 );
setTimeout(() =>
{
	clearInterval( inv );
}, 2000 );
