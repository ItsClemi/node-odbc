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
    eSqlType[eSqlType["eUint32"] = 5] = "eUint32";
    eSqlType[eSqlType["eBigInt"] = 6] = "eBigInt";
    eSqlType[eSqlType["eReal"] = 7] = "eReal";
    eSqlType[eSqlType["eChar"] = 8] = "eChar";
    eSqlType[eSqlType["eNChar"] = 9] = "eNChar";
    eSqlType[eSqlType["eVarChar"] = 10] = "eVarChar";
    eSqlType[eSqlType["eNVarChar"] = 11] = "eNVarChar";
    eSqlType[eSqlType["eBinary"] = 12] = "eBinary";
    eSqlType[eSqlType["eVarBinary"] = 13] = "eVarBinary";
    eSqlType[eSqlType["eDate"] = 14] = "eDate";
    eSqlType[eSqlType["eTimestamp"] = 15] = "eTimestamp";
    eSqlType[eSqlType["eNumeric"] = 16] = "eNumeric";
    eSqlType[eSqlType["eLongVarChar"] = 17] = "eLongVarChar";
    eSqlType[eSqlType["eLongNVarChar"] = 18] = "eLongNVarChar";
    eSqlType[eSqlType["eLongVarBinary"] = 19] = "eLongVarBinary";
    eSqlType[eSqlType["eSqlOutputVar"] = 20] = "eSqlOutputVar";
})(eSqlType = exports.eSqlType || (exports.eSqlType = {}));
class SqlStream {
    constructor(stream, length) {
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
        let types = this.prepareSqlParameters(...args);
        return this._connection.prepareQuery(query, types, ...args);
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
        let types = this.prepareSqlParameters(...args);
        this._connection.executeQuery(eFetchOperation, cb, query, types, ...args);
    }
    getInfo() {
        return this._connection.getInfo();
    }
    prepareSqlParameters(...args) {
        let types = new Array(args.length);
        let i = 0;
        try {
            for (; i < args.length; i++) {
                types[i] = getParameterType(args[i]);
            }
        }
        catch (err) {
            throw new Error(`failed to prepare parameter at: ${i}, ${err.Message}`);
        }
        return types;
    }
    transformParameters(...args) {
        const params = new Array(args.length * 2);
        for (let i = 0; i < args.length; i++) {
            params[i] = getParameterType(args[i]);
            params[i + 1] = args[i];
        }
        return params;
    }
}
exports.Connection = Connection;
function makeInputStream(stream, length) {
    return new SqlStream(stream, length);
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
    asUInt(reference) {
        return new SqlOutputParameter(reference, 5 /* eUint32 */);
    },
    asBigInt(reference) {
        return new SqlOutputParameter(reference, 6 /* eBigInt */);
    },
    asReal(reference) {
        return new SqlOutputParameter(reference, 7 /* eReal */);
    },
    asChar(reference, length) {
        return new SqlOutputParameter(reference, 8 /* eChar */, length);
    },
    asNChar(reference, length) {
        return new SqlOutputParameter(reference, 9 /* eNChar */, length);
    },
    asVarChar(reference, length) {
        return new SqlOutputParameter(reference, 10 /* eVarChar */, length);
    },
    asNVarChar(reference, length) {
        return new SqlOutputParameter(reference, 11 /* eNVarChar */, length);
    },
    asBinary(reference, length) {
        return new SqlOutputParameter(reference, 12 /* eBinary */, length);
    },
    asVarBinary(reference, length) {
        return new SqlOutputParameter(reference, 13 /* eVarBinary */, length);
    },
    asDate(reference, scale) {
        return new SqlOutputParameter(reference, 14 /* eDate */, undefined, undefined, scale);
    },
    asTimestamp(reference, scale) {
        return new SqlOutputParameter(reference, 15 /* eTimestamp */, undefined, undefined, scale);
    },
    asNumeric(reference, precision, scale) {
        return new SqlOutputParameter(reference, 16 /* eNumeric */, undefined, precision, scale);
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
        const kMaxUInt32 = 0xFFFFFFFF;
        if (isNaN(i) || isFinite(i)) {
            throw new Error(`number isNan or isFinite ${i}`);
        }
        let even = (i | 0) === i;
        if (even && i !== -0.0 && i >= kMinInt && i <= kMaxInt) {
            return 4 /* eInt32 */;
        }
        else if (even && i !== -0.0 && i >= 0 && i <= kMaxUInt32) {
            return 5 /* eUint32 */;
        }
        else if (even && i !== -0.0) {
            return 6 /* eBigInt */;
        }
        else {
            return 7 /* eReal */;
        }
    }
    else if (typeof (i) == "string") {
        return 11 /* eNVarChar */;
    }
    else if (i instanceof Date) {
        return 14 /* eDate */;
    }
    else if (i instanceof Buffer) {
        return 12 /* eBinary */;
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
            if (i.paramType < 0 /* eNull */ || i.paramType > 20 /* eSqlOutputVar */) {
                throw new Error(`paramType: Invalid value range: Expected ${0 /* eNull */}-${20 /* eSqlOutputVar */} ${i.paramType}`);
            }
        }
        return 20 /* eSqlOutputVar */;
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
        return 13 /* eVarBinary */;
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
        return 16 /* eNumeric */;
    }
    else if (i instanceof SqlTimestamp) {
        return 15 /* eTimestamp */;
    }
    throw new Error(`invalid param type got: ${i}(${typeof (i)})`);
}
