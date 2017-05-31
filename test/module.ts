import * as odbc from "../lib/node-odbc";
import * as assert from "assert";

const dbconf = require( "./dbconf.json" );

export declare type ConnectionEntry = {
	name: string;
	connectionString: string;
	timeout: number;
}; 

export default {
	connection: <Array<ConnectionEntry>>( dbconf.Connections ),
};