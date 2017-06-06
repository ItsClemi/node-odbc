"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const path = require("path");
const fs = require("fs");
exports.outputName = "node-odbc.node";
function getBinaryName() {
    let platform = process.platform;
    let binaryName = [
        platform, '-',
        process.arch, '-',
        process.versions.modules
    ].join('');
    return [binaryName, exports.outputName].join('_');
}
exports.getBinaryName = getBinaryName;
function getBinaryPath() {
    const defaultBinaryPath = path.join(__dirname, '..', 'vendor');
    return path.join(defaultBinaryPath, getBinaryName().replace(/_(?=node-odbc\.node)/, '/'));
}
exports.getBinaryPath = getBinaryPath;
function requireBinding() {
    if (!fs.existsSync(getBinaryPath())) {
        throw new Error("cannot find node-odbc.node binary!");
    }
    return require(getBinaryPath());
}
exports.requireBinding = requireBinding;
