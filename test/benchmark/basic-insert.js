"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const odbc = require("../../lib/node-odbc");
const mod = require("../module");
const async = require("async");
let _con;
let tableName;
exports.default = {
    name: "default insert (10000 * 5)",
    setup: function (done) {
        tableName = `tblTestInsert${Math.floor(Math.random() * 1000)}`;
        _con = new odbc.Connection()
            .connect(mod.connection[0].connectionString);
        _con.executeQuery(0 /* eSingle */, (res, err) => {
            if (err) {
                console.log(err);
                throw err;
            }
            console.log(`tbl created ${tableName}`);
            done();
        }, `CREATE TABLE ${tableName}(a varchar( 256 ) NOT NULL, b nvarchar( 128 ) NOT NULL, c int NOT NULL, d bigint NOT NULL, e real NOT NULL)`);
    },
    exec: function (done) {
        async.times(10000, (n, next) => {
            _con.executeQuery((res, err) => {
                if (err) {
                    console.log(err);
                }
                next();
            }, `INSERT INTO ${tableName}(a, b, c, d, e )VALUES(?, ?, ?, ?, ?)`, "Hello World!", "Unicode: \u01F8\u01EA\u01F7\u01B9\u007E\u00F4\u0110\u0243\u0187", Math.floor(Math.random() * 250), Math.floor((Math.random() * 4294967296) + 4294967296), Math.random() * 100);
        }, done);
    },
    teardown: () => {
        _con.executeQuery((res, err) => {
            if (err != null) {
                throw err;
            }
        }, `DROP TABLE ${tableName}`);
    },
};
