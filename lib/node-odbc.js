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
class SqlStream {
    constructor(type, stream, length) {
        if (exports.enableValidation) {
            if (type != 17 /* eLongNVarChar */ && type != 18 /* eLongVarBinary */) {
                throw new TypeError(`type: out of range ${type}`);
            }
            if (length > 0xFFFFFFFF) {
                throw new Error(`length: odbc only supports 32 bit length ${length}`);
            }
        }
        this.type = type;
        this.stream = stream;
        this.length = length;
    }
}
exports.SqlStream = SqlStream;
;
class SqlNumeric {
    constructor(precision, scale, sign, value) {
        this.precision = precision;
        this.scale = scale;
        this.sign = sign;
        this.value = value;
    }
}
exports.SqlNumeric = SqlNumeric;
;
class SqlTimestamp extends Date {
}
exports.SqlTimestamp = SqlTimestamp;
class SqlOutputParameter {
    constructor(reference, paramType, length, precision, scale) {
        this.reference = reference;
        this.paramType = paramType;
        this.length = length || 0;
        this.precision = precision || 0;
        this.scale = scale || 0;
    }
}
exports.SqlOutputParameter = SqlOutputParameter;
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
        return this._connection.prepareQuery(query, this.transformParameters(...args));
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
        this._connection.executeQuery(eFetchOperation, cb, query, this.transformParameters(...args));
    }
    getInfo() {
        return this._connection.getInfo();
    }
    transformParameters(...args) {
        const params = new Array(args.length * 2);
        let i = 0;
        try {
            for (; i < args.length; i++) {
                params[(i * 2)] = getParameterType(args[i]);
                params[(i * 2) + 1] = args[i];
            }
        }
        catch (err) {
            throw new Error(`failed to prepare parameter at: ${i}, ${err.Message}`);
        }
        return params;
    }
}
exports.Connection = Connection;
function makeInputStream(type, stream, length) {
    return new SqlStream(type, stream, length);
}
exports.makeInputStream = makeInputStream;
function makeNumeric(precision, scale, sign, value) {
    return new SqlNumeric(precision, scale, sign, value);
}
exports.makeNumeric = makeNumeric;
function makeTimestamp(date) {
    return new SqlTimestamp(date);
}
exports.makeTimestamp = makeTimestamp;
exports.SqlOutput = {
    asBitOutput(reference) {
        return new SqlOutputParameter(reference, 1 /* eBit */);
    },
    asTinyint(reference) {
        return new SqlOutputParameter(reference, 2 /* eTinyint */);
    },
    asSmallint(reference) {
        return new SqlOutputParameter(reference, 3 /* eSmallint */);
    },
    asInt(reference) {
        return new SqlOutputParameter(reference, 4 /* eInt32 */);
    },
    asBigInt(reference) {
        return new SqlOutputParameter(reference, 5 /* eBigInt */);
    },
    asReal(reference) {
        return new SqlOutputParameter(reference, 6 /* eReal */);
    },
    asChar(reference, length) {
        return new SqlOutputParameter(reference, 7 /* eChar */, length);
    },
    asNChar(reference, length) {
        return new SqlOutputParameter(reference, 8 /* eNChar */, length);
    },
    asVarChar(reference, length) {
        return new SqlOutputParameter(reference, 9 /* eVarChar */, length);
    },
    asNVarChar(reference, length) {
        return new SqlOutputParameter(reference, 10 /* eNVarChar */, length);
    },
    asBinary(reference, length) {
        return new SqlOutputParameter(reference, 11 /* eBinary */, length);
    },
    asVarBinary(reference, length) {
        return new SqlOutputParameter(reference, 12 /* eVarBinary */, length);
    },
    asDate(reference, scale) {
        return new SqlOutputParameter(reference, 13 /* eDate */, undefined, undefined, scale);
    },
    asTimestamp(reference, scale) {
        return new SqlOutputParameter(reference, 14 /* eTimestamp */, undefined, undefined, scale);
    },
    asNumeric(reference, precision, scale) {
        return new SqlOutputParameter(reference, 15 /* eNumeric */, undefined, precision, scale);
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
function getParameterType(i) {
    if (i == null) {
        return 0 /* eNull */;
    }
    else if (typeof (i) == "boolean") {
        return 1 /* eBit */;
    }
    else if (typeof (i) == "number") {
        const kMaxInt = 0x7FFFFFFF;
        const kMinInt = -kMaxInt - 1;
        if (isNaN(i) || !isFinite(i)) {
            throw new Error(`number isNan or not isFinite ${i}`);
        }
        let even = Math.floor(i) === i;
        if (even && i !== -0.0 && i >= kMinInt && i <= kMaxInt) {
            return 4 /* eInt32 */;
        }
        else if (even && i !== -0.0) {
            return 5 /* eBigInt */;
        }
        else {
            return 6 /* eReal */;
        }
    }
    else if (typeof (i) == "string") {
        return 10 /* eNVarChar */;
    }
    else if (i instanceof Date) {
        if (isNaN(i.getMilliseconds())) {
            throw new TypeError(`date: value is NaN ${i.getMilliseconds()}`);
        }
        return 13 /* eDate */;
    }
    else if (i instanceof Buffer) {
        return 12 /* eVarBinary */;
    }
    else if (i instanceof SqlOutputParameter) {
        if (exports.enableValidation) {
            if (i.reference == undefined) {
                throw new TypeError(`reference: Expected SqlType, got ${i.reference}(${typeof (i.reference)})`);
            }
            if (typeof (i.paramType) != "number") {
                throw new TypeError(`paramType: Expected number, got ${typeof (i.paramType)}`);
            }
            if (i.length != undefined && typeof (i.length) != "number") {
                throw new TypeError(`length: Expected number, got ${typeof (i.length)}`);
            }
            if (i.precision != undefined && typeof (i.precision) != "number") {
                throw new TypeError(`precision: Expected number, got ${typeof (i.precision)}`);
            }
            if (i.scale != undefined && typeof (i.scale) != "number") {
                throw new TypeError(`scale: Expected number, got ${typeof (i.scale)}`);
            }
            if (i.paramType < 0 /* eNull */ || i.paramType > 19 /* eSqlOutputVar */) {
                throw new Error(`paramType: Invalid value range: Expected ${0 /* eNull */}-${19 /* eSqlOutputVar */} ${i.paramType}`);
            }
        }
        return 19 /* eSqlOutputVar */;
    }
    else if (i instanceof SqlStream) {
        if (exports.enableValidation) {
            if (typeof (i.stream) != "object") {
                throw new TypeError(`stream: Expected stream, got ${typeof (i.stream)}`);
            }
            if (typeof (i.length) != "number") {
                throw new TypeError(`length: Expected number, got ${typeof (i.length)}`);
            }
        }
        return 18 /* eLongVarBinary */;
    }
    else if (i instanceof SqlNumeric) {
        if (exports.enableValidation) {
            if (typeof (i.precision) != "number") {
                throw new TypeError(`precision: Expected number, got ${typeof (i.precision)}`);
            }
            if (typeof (i.scale) != "number") {
                throw new TypeError(`scale: Expected number, got ${typeof (i.scale)}`);
            }
            if (typeof (i.sign) != "boolean") {
                throw new TypeError(`sign: Expected boolean, got ${typeof (i.sign)}`);
            }
            if (typeof (i.value) != "object" && !(i.value instanceof Uint8Array)) {
                throw new TypeError(`value: Expected Uint8Array, got ${typeof (i.value)}`);
            }
            if (i.precision < 0 || i.precision > 255 || i.scale < 0 || i.scale > 127 || i.value.length > 16) {
                throw new Error(`invalid parameter range ${i}`);
            }
        }
        return 15 /* eNumeric */;
    }
    else if (i instanceof SqlTimestamp) {
        return 14 /* eTimestamp */;
    }
    throw new Error(`invalid param type got: ${i}(${typeof (i)})`);
}
