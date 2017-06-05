"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
function downloadBinary(url, dest, cb) {
}
function testAndDownloadBinary() {
    if (process.env.SKIP_NODE_ODBC_BINARY_DOWNLOAD_ON_CI) {
        console.log("Skipping downloading binaries on CI builds");
        return;
    }
}
