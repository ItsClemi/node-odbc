import * as odbc from "../lib/node-odbc";
import * as assert from "assert";
import * as mocha from "mocha";
import * as path from "path";
import * as fs from "fs";
import * as bench from "./bench";



function runMotcha()
{
	let tests = new mocha();

	tests.checkLeaks();

	tests.addFile( "./test/api.js" );
	tests.addFile( "./test/connection.js" );
	//tests.addFile( "./test/pooltest.js" );

	//tests.addFile( "./test/insert.js" );


	tests.run();
}

function runBenchmark()
{
	console.log( "running benchmark" );

	const insert = require( "./insert" );

	bench.runBenchmark( insert );

	

}

//runMotcha();


runBenchmark();



global.gc();


//> don't close console window
let inv = setInterval(() => { }, 1000 );
setTimeout(() =>
{
	clearInterval( inv );
}, 2000 );
