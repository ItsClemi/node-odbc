"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const path = require("path");
const fs = require("fs");
const os = require("os");
const pkg = require("../package.json");
exports.outputName = "node-odbc.node";
function getDefaultPath() {
    return path.join(__dirname, '..', 'vendor');
}
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
    return path.join(getDefaultPath(), getBinaryName().replace(/_(?=node-odbc\.node)/, '/'));
}
exports.getBinaryPath = getBinaryPath;
function requireBinding() {
    if (!fs.existsSync(getBinaryPath())) {
        try {
            return require(path.join(getDefaultPath(), exports.outputName));
        }
        catch (err) {
            try {
                return require(path.join(getDefaultPath(), "node-odbc.node"));
            }
            catch (errdev) {
                throw new Error(`cannot find node-odbc.node binary! ${os.EOL}${err.toString()}${os.EOL}${errdev.toString()}`);
            }
        }
    }
    return require(getBinaryPath());
}
exports.requireBinding = requireBinding;
function getBinaryUrl() {
    let site = "https://github.com/ItsClemi/node-odbc/releases/download";
    return [site, pkg.version, getBinaryName()].join('/');
}
exports.getBinaryUrl = getBinaryUrl;
