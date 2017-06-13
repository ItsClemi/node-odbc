"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const fs = require("fs");
const mkdir = require("mkdirp");
const path = require("path");
const spawn = require("cross-spawn");
const binding = require("../lib/binding");
function afterBuild(options) {
    var install = binding.getBinaryPath();
    var target = path.join(__dirname, '..', 'build', options.debug ? 'Debug' :
        process.config.target_defaults
            ? process.config.target_defaults.default_configuration
            : 'Release', binding.outputName);
    mkdir(path.dirname(install), function (err) {
        if (err && err.code !== 'EEXIST') {
            console.error(err.message);
            return;
        }
        fs.stat(target, function (err) {
            if (err) {
                console.error('Build succeeded but target not found');
                return;
            }
            fs.rename(target, install, function (err) {
                if (err) {
                    console.error(err.message);
                    return;
                }
                console.log('Installed to', install);
            });
        });
    });
}
function build(options) {
    var args = [require.resolve(path.join('node-gyp', 'bin', 'node-gyp.js')), 'rebuild'];
    console.log('Building:', [process.execPath].concat(args).join(' '));
    var proc = spawn(process.execPath, args, {
        stdio: [0, 1, 2]
    });
    proc.on('exit', function (errorCode) {
        if (!errorCode) {
            afterBuild(options);
            return;
        }
        if (errorCode === 127) {
            console.error('node-gyp not found!');
        }
        else {
            console.error('Build failed with error code:', errorCode);
        }
        process.exit(1);
    });
}
function parseArgs(args) {
    var options = {
        arch: process.arch,
        platform: process.platform,
        force: process.env.npm_config_force === 'true',
        debug: false,
        args: {},
    };
    options.args = args.filter((arg) => {
        if (arg === '-f' || arg === '--force') {
            options.force = true;
            return false;
        }
        else if (arg.substring(0, 13) === '--target_arch') {
            options.arch = arg.substring(14);
        }
        else if (arg === '-d' || arg === '--debug') {
            options.debug = true;
        }
        return true;
    });
    return options;
}
build(parseArgs(process.argv.slice(2)));
