import * as path from "path";
import * as fs from "fs";
import * as os from "os";

const pkg = require( "../package.json" );

export var outputName = "node-odbc.node";

function getDefaultPath()
{
	return path.join( __dirname, '..', 'vendor' );
}

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
	return path.join( getDefaultPath(), getBinaryName().replace( /_(?=node-odbc\.node)/, '/' ) );
}

export function requireBinding()
{
	if( !fs.existsSync( getBinaryPath() ) )
	{
		try
		{
			return require( path.join( getDefaultPath(), outputName ) );
		}
		catch( err )
		{
			try
			{
				return require( path.join( getDefaultPath(), "node-odbc.node" ) );
			}
			catch( errdev )
			{
				throw new Error( `cannot find node-odbc.node binary! ${os.EOL}${err.toString()}${os.EOL}${errdev.toString()}` );
			}
		}
	}

	return require( getBinaryPath() );
}


export function getBinaryUrl()
{
	let site = "https://github.com/ItsClemi/node-odbc/releases/download";

	return [site, pkg.version, getBinaryName()].join( '/' );
}