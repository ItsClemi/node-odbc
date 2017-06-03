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
var eFetchMode;
(function (eFetchMode) {
    eFetchMode[eFetchMode["eSingle"] = 0] = "eSingle";
    eFetchMode[eFetchMode["eArray"] = 1] = "eArray";
})(eFetchMode = exports.eFetchMode || (exports.eFetchMode = {}));
;
const ID_INPUT_STREAM = 0;
const ID_NUMERIC_VALUE = 1;
const ID_TIMESTAMP_VALUE = 2;
function makeInputStream(stream, length) {
    return { _typeId: ID_INPUT_STREAM, stream: stream, length: length };
}
exports.makeInputStream = makeInputStream;
function makeNumericValue(precision, scale, sign, value) {
    return { _typeId: ID_NUMERIC_VALUE, precision: precision, scale: scale, sign: sign, value: value };
}
exports.makeNumericValue = makeNumericValue;
function makeTimestampValue(date) {
    return { _typeId: ID_TIMESTAMP_VALUE, date: date };
}
exports.makeTimestampValue = makeTimestampValue;
//inject node-odbc types in this module scope
exports = Object.assign(exports, require("../bin/node-odbc.node"));
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
exports.setWriteStreamInitializer((targetStream, query) => {
    let writer = new SqlStreamWriter(query);
    targetStream.pipe(writer);
});
exports.setReadStreamInitializer((query, column) => {
    return new SqlStreamReader(query, column);
});
exports.setPromiseInitializer((query) => {
    return new bluebird((resolve, reject) => {
        query.setPromiseInfo(resolve, reject);
    });
});
///ext
//> from https://github.com/aspnet/EntityFramework/blob/f386095005e46ea3aa4d677e4439cdac113dbfb1/src/EFCore.SqlServer/Storage/Internal/SqlServerTransientExceptionDetector.cs
function getMssqlAzureReconnectStrategy() {
    return {
        retries: 5,
        errorCodes: [
            // SQL Error Code: 49920
            // Cannot process request. Too many operations in progress for subscription "%ld".
            // The service is busy processing multiple requests for this subscription.
            // Requests are currently blocked for resource optimization. Query sys.dm_operation_status for operation status.
            // Wait until pending requests are complete or delete one of your pending requests and retry your request later.
            49920,
            // SQL Error Code: 49919
            // Cannot process create or update request. Too many create or update operations in progress for subscription "%ld".
            // The service is busy processing multiple create or update requests for your subscription or server.
            // Requests are currently blocked for resource optimization. Query sys.dm_operation_status for pending operations.
            // Wait till pending create or update requests are complete or delete one of your pending requests and
            // retry your request later.
            49919,
            // SQL Error Code: 49918
            // Cannot process request. Not enough resources to process request.
            // The service is currently busy.Please retry the request later.
            49918,
            // SQL Error Code: 41839
            // Transaction exceeded the maximum number of commit dependencies.
            41839,
            // SQL Error Code: 41325
            // The current transaction failed to commit due to a serializable validation failure.
            41325,
            // SQL Error Code: 41305
            // The current transaction failed to commit due to a repeatable read validation failure.
            41305,
            // SQL Error Code: 41302
            // The current transaction attempted to update a record that has been updated since the transaction started.
            41302,
            // SQL Error Code: 41301
            // Dependency failure: a dependency was taken on another transaction that later failed to commit.
            41301,
            // SQL Error Code: 40613
            // Database XXXX on server YYYY is not currently available. Please retry the connection later.
            // If the problem persists, contact customer support, and provide them the session tracing ID of ZZZZZ.
            40613,
            // SQL Error Code: 40501
            // The service is currently busy. Retry the request after 10 seconds. Code: (reason code to be decoded).
            40501,
            // SQL Error Code: 40197
            // The service has encountered an error processing your request. Please try again.
            40197,
            // SQL Error Code: 10929
            // Resource ID: %d. The %s minimum guarantee is %d, maximum limit is %d and the current usage for the database is %d.
            // However, the server is currently too busy to support requests greater than %d for this database.
            // For more information, see http://go.microsoft.com/fwlink/?LinkId=267637. Otherwise, please try again.
            10929,
            // SQL Error Code: 10928
            // Resource ID: %d. The %s limit for the database is %d and has been reached. For more information,
            // see http://go.microsoft.com/fwlink/?LinkId=267637.
            10928,
            // SQL Error Code: 10060
            // A network-related or instance-specific error occurred while establishing a connection to SQL Server.
            // The server was not found or was not accessible. Verify that the instance name is correct and that SQL Server
            // is configured to allow remote connections. (provider: TCP Provider, error: 0 - A connection attempt failed
            // because the connected party did not properly respond after a period of time, or established connection failed
            // because connected host has failed to respond.)"}
            10060,
            // SQL Error Code: 10054
            // A transport-level error has occurred when sending the request to the server.
            // (provider: TCP Provider, error: 0 - An existing connection was forcibly closed by the remote host.)
            10054,
            // SQL Error Code: 10053
            // A transport-level error has occurred when receiving results from the server.
            // An established connection was aborted by the software in your host machine.
            10053,
            // SQL Error Code: 1205
            // Deadlock
            1205,
            // SQL Error Code: 233
            // The client was unable to establish a connection because of an error during connection initialization process before login.
            // Possible causes include the following: the client tried to connect to an unsupported version of SQL Server;
            // the server was too busy to accept new connections; or there was a resource limitation (insufficient memory or maximum
            // allowed connections) on the server. (provider: TCP Provider, error: 0 - An existing connection was forcibly closed by
            // the remote host.)
            233,
            // SQL Error Code: 121
            // The semaphore timeout period has expired
            121,
            // SQL Error Code: 64
            // A connection was successfully established with the server, but then an error occurred during the login process.
            // (provider: TCP Provider, error: 0 - The specified network name is no longer available.)
            64,
            // DBNETLIB Error Code: 20
            // The instance of SQL Server you attempted to connect to does not support encryption.
            20,
        ]
    };
}
exports.getMssqlAzureReconnectStrategy = getMssqlAzureReconnectStrategy;
