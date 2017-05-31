import * as odbc from "../lib/node-odbc";
import * as assert from "assert";
import * as mocha from "mocha";


let tests = new mocha();

tests.checkLeaks();

//tests.addFile( "./test/api.js" );
//tests.addFile( "./test/connection.js" );
tests.addFile( "./test/insert.js" );

tests.run();


//> don't close console window
//setInterval( () => { }, 1000 );
