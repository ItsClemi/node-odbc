import * as path from "path";
import * as fs from "fs";


export var outputName = "node-odbc.node";

export function getBinaryName()
{
	let platform = process.platform;

	let binaryName = [
		platform, '-',
		process.arch, '-',
		process.versions.modules
	].join( '' );

	return [binaryName, outputName].join( '_' );
}

export function getBinaryPath()
{
	const defaultBinaryPath = path.join( __dirname, '..', 'vendor' );

	return path.join( defaultBinaryPath, getBinaryName().replace( /_(?=node-odbc\.node)/, '/' ) );
}

export function requireBinding()
{
	if( !fs.existsSync( getBinaryPath() ) )
	{
		throw new Error( "cannot find node-odbc.node binary!" );
	}

	return require( getBinaryPath() );
}