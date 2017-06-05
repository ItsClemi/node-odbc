import * as fs from "fs";
import * as path from "path";
import * as proxy from "./helper/proxy";


function downloadBinary( url: string, dest: string, cb: () => void ): void
{

}




function testAndDownloadBinary()
{
	if( process.env.SKIP_NODE_ODBC_BINARY_DOWNLOAD_ON_CI )
	{
		console.log( "Skipping downloading binaries on CI builds" );
		return;
	}


}