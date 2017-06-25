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
var eSqlType;
(function (eSqlType) {
    eSqlType[eSqlType["eNull"] = 0] = "eNull";
    eSqlType[eSqlType["eBit"] = 1] = "eBit";
    eSqlType[eSqlType["eTinyint"] = 2] = "eTinyint";
    eSqlType[eSqlType["eSmallint"] = 3] = "eSmallint";
    eSqlType[eSqlType["eInt32"] = 4] = "eInt32";
    eSqlType[eSqlType["eBigInt"] = 5] = "eBigInt";
    eSqlType[eSqlType["eReal"] = 6] = "eReal";
    eSqlType[eSqlType["eChar"] = 7] = "eChar";
    eSqlType[eSqlType["eNChar"] = 8] = "eNChar";
    eSqlType[eSqlType["eVarChar"] = 9] = "eVarChar";
    eSqlType[eSqlType["eNVarChar"] = 10] = "eNVarChar";
    eSqlType[eSqlType["eBinary"] = 11] = "eBinary";
    eSqlType[eSqlType["eVarBinary"] = 12] = "eVarBinary";
    eSqlType[eSqlType["eDate"] = 13] = "eDate";
    eSqlType[eSqlType["eTimestamp"] = 14] = "eTimestamp";
    eSqlType[eSqlType["eNumeric"] = 15] = "eNumeric";
    eSqlType[eSqlType["eLongVarChar"] = 16] = "eLongVarChar";
    eSqlType[eSqlType["eLongNVarChar"] = 17] = "eLongNVarChar";
    eSqlType[eSqlType["eLongVarBinary"] = 18] = "eLongVarBinary";
    eSqlType[eSqlType["eSqlOutputVar"] = 19] = "eSqlOutputVar";
})(eSqlType = exports.eSqlType || (exports.eSqlType = {}));
const ID_INPUT_STREAM = 0;
const ID_NUMERIC_VALUE = 1;
const ID_TIMESTAMP_VALUE = 2;
const ID_OUTPUT_PARAMETER = 3;
var eFetchMode;
(function (eFetchMode) {
    eFetchMode[eFetchMode["eSingle"] = 0] = "eSingle";
    eFetchMode[eFetchMode["eArray"] = 1] = "eArray";
})(eFetchMode = exports.eFetchMode || (exports.eFetchMode = {}));
;
exports.enableValidation = true;
class Connection {
    constructor(advancedProps) {
        if (exports.enableValidation) {
            if (advancedProps) {
                if (typeof (advancedProps) != "object") {
                    throw new TypeError(`advancedProps: Expected object, got ${typeof (advancedProps)}`);
                }
                if (advancedProps.enableMssqlMars != undefined && typeof (advancedProps.enableMssqlMars) != "boolean") {
                    throw new TypeError(`enableMssqlMars: Expected boolean, got ${typeof (advancedProps.enableMssqlMars)}`);
                }
                if (advancedProps.poolSize != undefined && typeof (advancedProps.poolSize) != "number") {
                    throw new TypeError(`poolSize: Expected number, got ${typeof (advancedProps.poolSize)}`);
                }
            }
        }
        this._connection = new exports.IRawConnection(advancedProps);
    }
    connect(connectionString, connectionTimeout) {
        if (exports.enableValidation) {
            if (typeof (connectionString) != "string") {
                throw new TypeError(`connectionString: Expected string, got ${typeof (connectionString)}`);
            }
            if (connectionTimeout != undefined && typeof (connectionTimeout) != "number") {
                throw new TypeError(`connectionTimeout: Expected number, got ${typeof (connectionTimeout)}`);
            }
        }
        this._connection.connect(connectionString, connectionTimeout);
        return this;
    }
    disconnect(cb) {
        if (exports.enableValidation) {
            if (typeof (cb) != "function") {
                throw new TypeError(`cb: Expected function, got ${typeof (cb)}`);
            }
        }
        this._connection.disconnect(cb);
    }
    prepareQuery(query, ...args) {
        if (exports.enableValidation) {
            if (typeof (query) != "string") {
                throw new TypeError(`query: Expected string, got ${typeof (query)}`);
            }
        }
        return this._connection.prepareQuery(query, ...args);
    }
    executeQuery(eFetchOperation, cb, query, ...args) {
        if (exports.enableValidation) {
            if (typeof (eFetchOperation) != "number") {
                throw new TypeError(`eFetchOperation: Expected number, got ${typeof (cb)}`);
            }
            if (typeof (cb) != "function") {
                throw new TypeError(`cb: Expected function, got ${typeof (cb)}`);
            }
            if (typeof (query) != "string") {
                throw new TypeError(`query: Expected string, got ${typeof (query)}`);
            }
            if (eFetchOperation < 0 || eFetchOperation > 1 /* eArray */) {
                throw new Error(`eFetchOperation: Invalid value range: Expected {${0 /* eSingle */} - ${1 /* eArray */}} : got ${eFetchOperation}`);
            }
        }
        this._connection.executeQuery(eFetchOperation, cb, query, ...args);
    }
    getInfo() {
        return this._connection.getInfo();
    }
}
exports.Connection = Connection;
function makeInputStream(type, stream, length) {
    return { _typeId: ID_INPUT_STREAM, type, stream, length };
}
exports.makeInputStream = makeInputStream;
function makeNumeric(precision, scale, sign, value) {
    return { _typeId: ID_NUMERIC_VALUE, precision, scale, sign, value };
}
exports.makeNumeric = makeNumeric;
function makeTimestamp(date) {
    return { _typeId: ID_TIMESTAMP_VALUE, date };
}
exports.makeTimestamp = makeTimestamp;
function makeOutputParameter(reference, paramType, length, precision, scale) {
    return { _typeId: ID_OUTPUT_PARAMETER, reference, paramType, length: length || 0, precision: precision || 0, scale: scale || 0 };
}
exports.SqlOutput = {
    asBitOutput(reference) {
        return makeOutputParameter(reference, 1 /* eBit */);
    },
    asTinyint(reference) {
        return makeOutputParameter(reference, 2 /* eTinyint */);
    },
    asSmallint(reference) {
        return makeOutputParameter(reference, 3 /* eSmallint */);
    },
    asInt(reference) {
        return makeOutputParameter(reference, 4 /* eInt32 */);
    },
    asBigInt(reference) {
        return makeOutputParameter(reference, 5 /* eBigInt */);
    },
    asReal(reference) {
        return makeOutputParameter(reference, 6 /* eReal */);
    },
    asChar(reference, length) {
        return makeOutputParameter(reference, 7 /* eChar */, length);
    },
    asNChar(reference, length) {
        return makeOutputParameter(reference, 8 /* eNChar */, length);
    },
    asVarChar(reference, length) {
        return makeOutputParameter(reference, 9 /* eVarChar */, length);
    },
    asNVarChar(reference, length) {
        return makeOutputParameter(reference, 10 /* eNVarChar */, length);
    },
    asBinary(reference, length) {
        return makeOutputParameter(reference, 11 /* eBinary */, length);
    },
    asVarBinary(reference, length) {
        return makeOutputParameter(reference, 12 /* eVarBinary */, length);
    },
    asDate(reference, scale) {
        return makeOutputParameter(reference, 13 /* eDate */, undefined, undefined, scale);
    },
    asTimestamp(reference, scale) {
        return makeOutputParameter(reference, 14 /* eTimestamp */, undefined, undefined, scale);
    },
    asNumeric(reference, precision, scale) {
        return makeOutputParameter(reference, 15 /* eNumeric */, undefined, precision, scale);
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
