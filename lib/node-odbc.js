"use strict";
// File: node-odbc.ts
// 
// node-odbc (odbc interface for NodeJS)
// 
// Copyright 2017 Clemens Susenbeth
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
Object.defineProperty(exports, "__esModule", { value: true });
const stream = require("stream");
const bluebird = require("bluebird");
const binding = require("./binding");
//> type helpers
const ID_INPUT_STREAM = 0;
const ID_NUMERIC_VALUE = 1;
const ID_DATE_VALUE = 2;
function makeInputStream(stream, length) {
    return { _typeId: ID_INPUT_STREAM, stream: stream, length: length };
}
exports.makeInputStream = makeInputStream;
function makeNumericValue(precision, scale, sign, value) {
    return { _typeId: ID_NUMERIC_VALUE, precision: precision, scale: scale, sign: sign, value: value };
}
exports.makeNumericValue = makeNumericValue;
function makeDateValue(date) {
    return { _typeId: ID_DATE_VALUE, date: date };
}
exports.makeDateValue = makeDateValue;
var eFetchMode;
(function (eFetchMode) {
    eFetchMode[eFetchMode["eSingle"] = 0] = "eSingle";
    eFetchMode[eFetchMode["eArray"] = 1] = "eArray";
})(eFetchMode = exports.eFetchMode || (exports.eFetchMode = {}));
;
class SqlStreamReader extends stream.Readable {
    constructor(_query, _column) {
        super();
        this.query = _query;
        this.column = _column;
    }
    _read() {
        exports.requestNextChunk(this.query, this.column, (chunk) => {
            this.push(chunk);
        });
    }
}
class SqlStreamWriter extends stream.Writable {
    constructor(query) {
        super();
        this._query = query;
    }
    _write(chunk, encoding, next) {
        exports.processNextChunk(this._query, chunk, (error) => {
            next(error);
        });
    }
}
//> ../vendor/node-odbc.node
exports = Object.assign(exports, binding.requireBinding());
exports.setWriteStreamInitializer((targetStream, query) => {
    targetStream.pipe(new SqlStreamWriter(query));
});
exports.setReadStreamInitializer((query, column) => {
    return new SqlStreamReader(query, column);
});
exports.setPromiseInitializer((query) => {
    return new bluebird((resolve, reject) => {
        query.setPromiseInfo(resolve, reject);
    });
});
