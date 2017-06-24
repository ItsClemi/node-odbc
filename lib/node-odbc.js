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
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const stream = require("stream");
const bluebird = require("bluebird");
const binding = require("./binding");
var eFetchMode;
(function (eFetchMode) {
    eFetchMode[eFetchMode["eSingle"] = 0] = "eSingle";
    eFetchMode[eFetchMode["eArray"] = 1] = "eArray";
})(eFetchMode = exports.eFetchMode || (exports.eFetchMode = {}));
;
var eSqlOutputType;
(function (eSqlOutputType) {
    eSqlOutputType[eSqlOutputType["eBitOutput"] = 0] = "eBitOutput";
    eSqlOutputType[eSqlOutputType["eTinyintOutput"] = 1] = "eTinyintOutput";
    eSqlOutputType[eSqlOutputType["eSmallint"] = 2] = "eSmallint";
    eSqlOutputType[eSqlOutputType["eInt"] = 3] = "eInt";
    eSqlOutputType[eSqlOutputType["eUint32"] = 4] = "eUint32";
    eSqlOutputType[eSqlOutputType["eBigInt"] = 5] = "eBigInt";
    eSqlOutputType[eSqlOutputType["eFloat"] = 6] = "eFloat";
    eSqlOutputType[eSqlOutputType["eReal"] = 7] = "eReal";
    eSqlOutputType[eSqlOutputType["eChar"] = 8] = "eChar";
    eSqlOutputType[eSqlOutputType["eNChar"] = 9] = "eNChar";
    eSqlOutputType[eSqlOutputType["eVarChar"] = 10] = "eVarChar";
    eSqlOutputType[eSqlOutputType["eNVarChar"] = 11] = "eNVarChar";
    eSqlOutputType[eSqlOutputType["eBinary"] = 12] = "eBinary";
    eSqlOutputType[eSqlOutputType["eVarBinary"] = 13] = "eVarBinary";
    eSqlOutputType[eSqlOutputType["eDate"] = 14] = "eDate";
    eSqlOutputType[eSqlOutputType["eTimestamp"] = 15] = "eTimestamp";
    eSqlOutputType[eSqlOutputType["eNumeric"] = 16] = "eNumeric";
})(eSqlOutputType = exports.eSqlOutputType || (exports.eSqlOutputType = {}));
exports.enableValidation = true;
class Connection2 {
    constructor(advancedProps) {
        if (exports.enableValidation) {
            if (advancedProps) {
                if ((advancedProps.enableMssqlMars != undefined && typeof (advancedProps.enableMssqlMars) != "boolean") ||
                    (advancedProps.poolSize != undefined && typeof (advancedProps.poolSize) != "number")) {
                    throw new TypeError("advancedProps contains invalid type");
                }
            }
        }
        this._connection = new exports.Connection(advancedProps);
    }
    connect(connectionString, connectionTimeout) {
        if (exports.enableValidation) {
            if (typeof (connectionString) != "string") {
                throw new TypeError(`connectionString: invalid type ${connectionString}`);
            }
            if (connectionTimeout != undefined && typeof (connectionTimeout) != "number") {
                throw new TypeError("");
            }
        }
        this._connection.connect(connectionString, connectionTimeout);
        return this;
    }
}
exports.Connection2 = Connection2;
//> type helpers
const ID_INPUT_STREAM = 0;
const ID_NUMERIC_VALUE = 1;
const ID_DATE_VALUE = 2;
const ID_OUTPUT_PARAMETER = 3;
function makeInputStream(stream, length) {
    return { _typeId: ID_INPUT_STREAM, stream, length };
}
exports.makeInputStream = makeInputStream;
function makeNumeric(precision, scale, sign, value) {
    return { _typeId: ID_NUMERIC_VALUE, precision, scale, sign, value };
}
exports.makeNumeric = makeNumeric;
function makeTimestamp(date) {
    return { _typeId: ID_DATE_VALUE, date };
}
exports.makeTimestamp = makeTimestamp;
exports.SqlOutput = {
    asBitOutput(ref) {
        return { _typeId: ID_OUTPUT_PARAMETER, paramType: 0 /* eBitOutput */, ref };
    },
    asTinyint(ref) {
        return { _typeId: ID_OUTPUT_PARAMETER, paramType: 1 /* eTinyintOutput */, ref };
    },
    asSmallint(ref) {
        return { _typeId: ID_OUTPUT_PARAMETER, paramType: 2 /* eSmallint */, ref };
    },
    asInt(ref) {
        return { _typeId: ID_OUTPUT_PARAMETER, paramType: 3 /* eInt */, ref };
    },
    asBigInt(ref) {
        return { _typeId: ID_OUTPUT_PARAMETER, paramType: 5 /* eBigInt */, ref };
    },
    asFloat(ref) {
        return { _typeId: ID_OUTPUT_PARAMETER, paramType: 6 /* eFloat */, ref };
    },
    asReal(ref) {
        return { _typeId: ID_OUTPUT_PARAMETER, paramType: 7 /* eReal */, ref };
    },
    asChar(ref, length) {
        return { _typeId: ID_OUTPUT_PARAMETER, paramType: 8 /* eChar */, ref, length };
    },
    asNChar(ref, length) {
        return { _typeId: ID_OUTPUT_PARAMETER, paramType: 9 /* eNChar */, ref, length };
    },
    asVarChar(ref, length) {
        return { _typeId: ID_OUTPUT_PARAMETER, paramType: 10 /* eVarChar */, ref, length };
    },
    asNVarChar(ref, length) {
        return { _typeId: ID_OUTPUT_PARAMETER, paramType: 11 /* eNVarChar */, ref, length };
    },
    asBinary(ref, length) {
        return { _typeId: ID_OUTPUT_PARAMETER, paramType: 12 /* eBinary */, ref, length };
    },
    asVarBinary(ref, length) {
        return { _typeId: ID_OUTPUT_PARAMETER, paramType: 13 /* eVarBinary */, ref, length };
    },
    asDate(ref, scale) {
        return { _typeId: ID_OUTPUT_PARAMETER, paramType: 14 /* eDate */, ref, scale };
    },
    asTimestamp(ref, scale) {
        return { _typeId: ID_OUTPUT_PARAMETER, paramType: 15 /* eTimestamp */, ref, scale };
    },
    asNumeric(ref, precision, scale) {
        return { _typeId: ID_OUTPUT_PARAMETER, paramType: 15 /* eTimestamp */, ref, precision, scale };
    },
};
function getJSBridge() {
    return exports;
}
exports.getJSBridge = getJSBridge;
class SqlStreamReader extends stream.Readable {
    constructor(_query, _column) {
        super();
        this.query = _query;
        this.column = _column;
    }
    _read() {
        getJSBridge().requestNextChunk(this.query, this.column, (chunk) => {
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
        getJSBridge().processNextChunk(this._query, chunk, (error) => {
            next(error);
        });
    }
}
//> ../vendor/node-odbc.node
exports = Object.assign(exports, binding.requireBinding());
getJSBridge().setWriteStreamInitializer((targetStream, query) => {
    targetStream.pipe(new SqlStreamWriter(query));
});
getJSBridge().setReadStreamInitializer((query, column) => {
    return new SqlStreamReader(query, column);
});
getJSBridge().setPromiseInitializer((query) => {
    return new bluebird((resolve, reject) => {
        query.setPromiseInfo(resolve, reject);
    });
});
