"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const pkg = require("../package.json");
function getProxyConfig() {
    return process.env.npm_config_https_proxy ||
        process.env.npm_config_proxy ||
        process.env.npm_config_http_proxy ||
        '';
}
function getUserAgent() {
    return [
        'node/', process.version, ' ',
        'node-odbc-installer/', pkg.version
    ].join('');
}
function getProxySettings() {
    let options = {
        rejectUnauthorized: false,
        timeout: 60000,
        headers: {
            'User-Agent': getUserAgent(),
        },
        proxy: '',
    };
    let proxyConfig = getProxyConfig();
    if (proxyConfig) {
        options.proxy = proxyConfig;
    }
    return options;
}
exports.getProxySettings = getProxySettings;
