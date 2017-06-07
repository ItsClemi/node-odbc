import * as fs from "fs";
import * as path from "path";
import * as proxy from "./proxy";
import * as mkdir from "mkdirp";
import * as binding from "../lib/binding";
import * as os from "os";
import * as request from "request";
import * as log from "npmlog";


function downloadBinary( url: string, dest: string, cb: (err?) => void ): void
{
    var reportError = function(err) {
        var timeoutMessge;

        if (err.code === 'ETIMEDOUT') {
            if (err.connect === true) {
                timeoutMessge = 'Timed out attemping to establish a remote connection';
            } else {
                timeoutMessge = 'Timed out whilst downloading the prebuilt binary';
            }
        }
        cb(['Cannot download "', url, '": ', os.EOL, os.EOL,
            typeof err.message === 'string' ? err.message : err, os.EOL, os.EOL,
            timeoutMessge ? timeoutMessge + os.EOL + os.EOL : timeoutMessge,
            'Hint: If github.com is not accessible in your location', os.EOL,
            '      try setting a proxy via HTTP_PROXY, e.g. ', os.EOL, os.EOL,
            '      export HTTP_PROXY=http://example.com:1234', os.EOL, os.EOL,
            'or configure npm proxy via', os.EOL, os.EOL,
            '      npm config set proxy http://example.com:8080'].join(''));
    };

    var successful = function(response) {
        return response.statusCode >= 200 && response.statusCode < 300;
    };

    console.log('Downloading binary from', url);

    try {
        request(url, proxy.getProxySettings(), function(err, response) {
            if (err) {
                reportError(err);
            } else if (!successful(response)) {
                reportError(['HTTP error', response.statusCode, response.statusMessage].join(' '));
            } else {
                console.log('Download complete');
                cb();
            }
        })
            .on('response', function(response) {
                var length = parseInt(response.headers['content-length'], 10);
                var progress = log.newItem('', length);

                if (successful(response)) {
                    response.pipe(fs.createWriteStream(dest));
                }

                // The `progress` is true by default. However if it has not
                // been explicitly set it's `undefined` which is considered
                // as far as npm is concerned.
                if (process.env.npm_config_progress === 'true') {
                    log.enableProgress();

                    response.on('data', function(chunk) {
                        progress.completeWork(chunk.length);
                    })
                        .on('end', progress.finish);
                }
            });
    } catch (err) {
        cb(err);
    }

}

function testAndDownloadBinary()
{
	if( process.env.SKIP_NODE_ODBC_BINARY_DOWNLOAD_ON_CI )
	{
		console.log( "Skipping downloading binaries on CI builds" );
		return;
	}

    let binaryPath = binding.getBinaryPath();
    try {
        mkdir.sync(path.dirname(binaryPath));
    } catch (err) {
        console.error(`Unable to save binary ${path.dirname(binaryPath)}: ${err}`);
        return;
    }

    downloadBinary(binding.getBinaryUrl(), binaryPath, function(err) {
        if (err) {
            console.error(err);
            return;
        }

        console.log(`Binary saved to ${binaryPath}`);
    });
}


testAndDownloadBinary();