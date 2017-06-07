import * as stream from "stream";
import * as fs from "fs";
export declare type SqlComplexType = {
    readonly _typeId: number;
};
export declare type SqlStream = SqlComplexType & {
    stream: stream.Readable | stream.Writable;
    length: number;
};
export declare type SqlNumeric = SqlComplexType & {
    precision: number;
    scale: number;
    sign: boolean;
    value: Uint8Array;
};
export declare type SqlTimestamp = SqlComplexType & {
    date: Date;
};
export declare type SqlError = {
    readonly message: string;
    readonly sqlState: string;
    readonly code: number;
};
export declare type SqlColumnMetaData = {
    readonly name: string;
    readonly size: number;
    readonly dataType: string;
    readonly digits: number;
    readonly nullable: boolean;
};
export declare type SqlResultExtension = {
    readonly $sqlReturnValue?: number;
    readonly $sqlMetaData?: Array<SqlColumnMetaData>;
};
export declare type SqlResult = SqlResultExtension & Partial<any>;
export declare type SqlPartialResult<T> = SqlResultExtension & Partial<T>;
export declare type SqlResultArray = SqlResultExtension & Array<any>;
export declare type SqlPartialResultArray<T> = SqlResultExtension & Array<T>;
export declare type SqlResultTypes = SqlResult | SqlResultArray;
export declare type SqlPartialResultTypes<T> = SqlPartialResult<T> & SqlPartialResultArray<T>;
export declare type SqlTypes = null | string | boolean | number | Date | Buffer | SqlStream | SqlNumeric | SqlTimestamp;
export declare function makeInputStream(stream: fs.ReadStream | stream.Readable, length: number): SqlStream;
export declare function makeNumericValue(precision: number, scale: number, sign: boolean, value: Uint8Array): SqlNumeric;
export declare function makeDateValue(date: Date): SqlTimestamp;
export interface IResilienceStrategy {
    retries: number;
    errorCodes: Array<number>;
}
export declare type ConnectionInfo = {
    driverName: string;
    driverVersion: string;
    databaseName: string;
    odbcVersion: string;
    dbmsName: string;
    internalServerType: number;
    odbcConnectionString: string;
    resilienceStrategy: IResilienceStrategy;
};
export declare type ConnectionProps = {
    enableMssqlMars?: boolean;
    poolSize?: number;
};
export declare const enum eFetchMode {
    eSingle = 0,
    eArray = 1,
}
export declare class Connection {
    constructor(advancedProps?: ConnectionProps);
    connect(connectionString: string, connectionTimeout?: number): Connection;
    disconnect(cb: () => void): void;
    prepareQuery(query: string, ...args: (SqlTypes)[]): ISqlQuery;
    executeQuery(cb: (result: SqlResult, error: SqlError) => void, query: string, ...args: (SqlTypes)[]): void;
    executeQuery(eFetchMode: eFetchMode, cb: (result: SqlResultTypes, error: SqlError) => void, query: string, ...args: (SqlTypes)[]): void;
    executeQuery<T>(cb: (result: SqlPartialResult<T>, error: SqlError) => void, query: string, ...args: (SqlTypes)[]): void;
    executeQuery<T>(eFetchMode: eFetchMode, cb: (result: SqlPartialResultTypes<T>, error: SqlError) => void, query: string, ...args: (SqlTypes)[]): void;
    getInfo(): ConnectionInfo;
}
export interface ISqlQuery {
    enableReturnValue(): ISqlQuery;
    enableMetaData(): ISqlQuery;
    setQueryTimeout(timeout: number): ISqlQuery;
    toSingle(cb: (result: SqlResult, error: SqlError) => void): void;
    toSingle<T>(cb: (result: SqlPartialResult<T>, error: SqlError) => void): void;
    toSingle(): Promise<SqlResult>;
    toSingle<T>(): Promise<SqlPartialResult<T>>;
    toArray(cb: (result: SqlResultArray, error: SqlError) => void): void;
    toArray<T>(cb: (result: SqlPartialResultArray<T>, error: SqlError) => void): void;
    toArray(): Promise<SqlResultArray>;
    toArray<T>(): Promise<SqlPartialResultArray<T>>;
}
export interface ISqlQueryEx extends ISqlQuery {
    setPromiseInfo(resolve: any, reject: any): void;
}
export declare function setWriteStreamInitializer(cb: (targetStream: stream.Readable, query: ISqlQueryEx) => void): void;
export declare function setPromiseInitializer<T>(cb: (query: ISqlQueryEx) => T): void;
export declare function setReadStreamInitializer(cb: (query: ISqlQueryEx, column: number) => stream.Readable): void;
export declare function processNextChunk(query: ISqlQueryEx, chunk: Int8Array, cb: (error) => void): void;
export declare function requestNextChunk(query: ISqlQueryEx, column: number, cb: (chunk: Int8Array) => void): Int8Array;
