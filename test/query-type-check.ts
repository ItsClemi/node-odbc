import * as assert from "assert";
import * as odbc from "../lib/node-odbc";
import * as mod from "./module";
import * as async from "async";


mod.connection.forEach((connection) => {
    describe(`query type checker insert / fetch (exec toSingle)`, function() {
        this.timeout(connection.timeout);

        afterEach(() => {
            global.gc();
        });

        it("insert null -> fetch null", (done) => {
            let tblName = mod.getRandomTableName();


            let con = new odbc.Connection()
                .connect(connection.connectionString);

            async.series([
                (cb) => {
                    con.executeQuery((res, err) => {
                        cb(err);
                    },
                        `CREATE TABLE ${tblName}( a int )`
                    )
                },

                (cb) => {
                    con.executeQuery(
                        (res, err) => {
                            cb(err);
                        },
                        `INSERT INTO ${tblName}(a)VALUES(?)`, null
                    );
                },

                (cb) => {
                    con.executeQuery(
                        (res, err) => {
                            assert(res.a == null);
                            cb(err);
                        },
                        `SELECT * FROM ${tblName}`
                    );
                }
            ], (err) => {
                if (err) {
                    done(err);
                }
                done();
            });
        });

        it("insert bool -> fetch bool", (done) => {
            let tblName = mod.getRandomTableName();

            let con = new odbc.Connection()
                .connect(connection.connectionString);

            let val = Math.random() > 0.5 ? true : false;


            async.series([
                (cb) => {
                    con.executeQuery((res, err) => {
                        cb(err);
                    },
                        `CREATE TABLE ${tblName}( a bit )`
                    )
                },

                (cb) => {
                    con.executeQuery(
                        (res, err) => {
                            cb(err);
                        },
                        `INSERT INTO ${tblName}(a)VALUES(?)`, val
                    );
                },

                (cb) => {
                    con.executeQuery(
                        (res, err) => {
                            assert(res.a == val);
                            cb(err);
                        },
                        `SELECT * FROM ${tblName}`
                    );
                }
            ], (err) => {
                if (err) {
                    done(err);
                }
                done();
            });
        });

        it("insert int32 -> fetch int32", (done) => {
            let tblName = mod.getRandomTableName();


            let con = new odbc.Connection()
                .connect(connection.connectionString);

            let num = mod.getRandomInt(250, 50);

            async.series([
                (cb) => {
                    con.executeQuery((res, err) => {
                        cb(err);
                    },
                        `CREATE TABLE ${tblName}( a int )`
                    )
                },

                (cb) => {
                    con.executeQuery(
                        (res, err) => {
                            cb(err);
                        },
                        `INSERT INTO ${tblName}(a)VALUES(?)`, num
                    );
                },

                (cb) => {
                    con.executeQuery(
                        (res, err) => {
                            assert(res.a == num);
                            cb(err);
                        },
                        `SELECT * FROM ${tblName}`
                    );
                }
            ], (err) => {
                if (err) {
                    done(err);
                }
                done();
            });
        });

        it("insert int64 -> fetch int64", (done) => {
            let tblName = mod.getRandomTableName();


            let con = new odbc.Connection()
                .connect(connection.connectionString);

            let num = mod.getRandomInt(4200000000, 2147483647);

            async.series([
                (cb) => {
                    con.executeQuery((res, err) => {
                        cb(err);
                    },
                        `CREATE TABLE ${tblName}( a bigint )`
                    )
                },

                (cb) => {
                    con.executeQuery(
                        (res, err) => {
                            cb(err);
                        },
                        `INSERT INTO ${tblName}(a)VALUES(?)`, num
                    );
                },

                (cb) => {
                    con.executeQuery(
                        (res, err) => {
                            assert(res.a == num);
                            cb(err);
                        },
                        `SELECT * FROM ${tblName}`
                    );
                }
            ], (err) => {
                if (err) {
                    done(err);
                }
                done();
            });
        });

        it("insert double -> fetch double", (done) => {
            let tblName = mod.getRandomTableName();


            let con = new odbc.Connection()
                .connect(connection.connectionString);

            let num = Math.random() * 250;

            async.series([
                (cb) => {
                    con.executeQuery((res, err) => {
                        cb(err);
                    },
                        `CREATE TABLE ${tblName}( a float )`
                    )
                },

                (cb) => {
                    con.executeQuery(
                        (res, err) => {
                            cb(err);
                        },
                        `INSERT INTO ${tblName}(a)VALUES(?)`, num
                    );
                },

                (cb) => {
                    con.executeQuery(
                        (res, err) => {
                            assert(res.a == num);
                            cb(err);
                        },
                        `SELECT * FROM ${tblName}`
                    );
                }
            ], (err) => {
                if (err) {
                    done(err);
                }
                done();
            });
        });

        it("insert string -> fetch string (char)", (done) => {
            let tblName = mod.getRandomTableName();


            let con = new odbc.Connection()
                .connect(connection.connectionString);

            let str = mod.getRandomTableName();

            async.series([
                (cb) => {
                    con.executeQuery((res, err) => {
                        cb(err);
                    },
                        `CREATE TABLE ${tblName}( a char(128) )`
                    )
                },

                (cb) => {
                    con.executeQuery(
                        (res, err) => {
                            cb(err);
                        },
                        `INSERT INTO ${tblName}(a)VALUES(?)`, str
                    );
                },

                (cb) => {
                    con.executeQuery(
                        (res, err) => {
                            for (var i = 0; i < 128; i++) {
                                let rc = (<string>res.a).charCodeAt(i);
                                let c;
                                if (i >= str.length) {
                                    c = " ".charCodeAt(0);
                                }
                                else {
                                    c = str.charCodeAt(i);
                                }
                                assert(rc == c);
                            }

                            cb(err);
                        },
                        `SELECT * FROM ${tblName}`
                    );
                }
            ], (err) => {
                if (err) {
                    done(err);
                }
                done();
            });
        });

        it("insert string -> fetch string (nchar)", (done) => {
            let tblName = mod.getRandomTableName();


            let con = new odbc.Connection()
                .connect(connection.connectionString);

            let str = mod.getRandomTableName();

            async.series([
                (cb) => {
                    con.executeQuery((res, err) => {
                        cb(err);
                    },
                        `CREATE TABLE ${tblName}( a nchar(128) )`
                    )
                },

                (cb) => {
                    con.executeQuery(
                        (res, err) => {
                            cb(err);
                        },
                        `INSERT INTO ${tblName}(a)VALUES(?)`, str
                    );
                },

                (cb) => {
                    con.executeQuery(
                        (res, err) => {
                            for (var i = 0; i < 128; i++) {
                                let rc = (<string>res.a).charCodeAt(i);
                                let c;
                                if (i >= str.length) {
                                    c = " ".charCodeAt(0);
                                }
                                else {
                                    c = str.charCodeAt(i);
                                }
                                assert(rc == c);
                            }

                            cb(err);
                        },
                        `SELECT * FROM ${tblName}`
                    );
                }
            ], (err) => {
                if (err) {
                    done(err);
                }
                done();
            });
        });

        it("insert string -> fetch string (varchar)", (done) => {
            let tblName = mod.getRandomTableName();


            let con = new odbc.Connection()
                .connect(connection.connectionString);

            let str = mod.getRandomTableName();

            async.series([
                (cb) => {
                    con.executeQuery((res, err) => {
                        cb(err);
                    },
                        `CREATE TABLE ${tblName}( a varchar(128) )`
                    )
                },

                (cb) => {
                    con.executeQuery(
                        (res, err) => {
                            cb(err);
                        },
                        `INSERT INTO ${tblName}(a)VALUES(?)`, str
                    );
                },

                (cb) => {
                    con.executeQuery(
                        (res, err) => {
                            assert(res.a == str);
                            cb(err);
                        },
                        `SELECT * FROM ${tblName}`
                    );
                }
            ], (err) => {
                if (err) {
                    done(err);
                }
                done();
            });
        });

        it("insert string -> fetch string (nvarchar)", (done) => {
            let tblName = mod.getRandomTableName();


            let con = new odbc.Connection()
                .connect(connection.connectionString);

            let str = mod.getRandomTableName();

            async.series([
                (cb) => {
                    con.executeQuery((res, err) => {
                        cb(err);
                    },
                        `CREATE TABLE ${tblName}( a nvarchar(128) )`
                    )
                },

                (cb) => {
                    con.executeQuery(
                        (res, err) => {
                            cb(err);
                        },
                        `INSERT INTO ${tblName}(a)VALUES(?)`, str
                    );
                },

                (cb) => {
                    con.executeQuery(
                        (res, err) => {
                            assert(res.a == str);
                            cb(err);
                        },
                        `SELECT * FROM ${tblName}`
                    );
                }
            ], (err) => {
                if (err) {
                    done(err);
                }
                done();
            });
        });

        it("insert date -> fetch date", (done) => {
            let tblName = mod.getRandomTableName();


            let con = new odbc.Connection()
                .connect(connection.connectionString);

            let date = new Date(Date.now());

            async.series([
                (cb) => {
                    con.executeQuery((res, err) => {
                        cb(err);
                    },
                        `CREATE TABLE ${tblName}( a datetime )`
                    )
                },

                (cb) => {
                    con.executeQuery(
                        (res, err) => {
                            cb(err);
                        },
                        `INSERT INTO ${tblName}(a)VALUES(?)`, odbc.makeDateValue(date)
                    );
                },

                (cb) => {
                    con.executeQuery(
                        (res, err) => {
                            assert((<Date>res.a).getFullYear() == date.getFullYear());
                            assert((<Date>res.a).getMonth() == date.getMonth());
                            assert((<Date>res.a).getDate() == date.getDate());

                            cb(err);
                        },
                        `SELECT * FROM ${tblName}`
                    );
                }
            ], (err) => {
                if (err) {
                    done(err);
                    return;
                }
                done();
            });
        });

        it("insert timestamp -> fetch date", (done) => {
            let tblName = mod.getRandomTableName();


            let con = new odbc.Connection()
                .connect(connection.connectionString);

            let date = new Date(2017, 6, 7, 5, 46, 12, 623);

            async.series([
                (cb) => {
                    con.executeQuery((res, err) => {
                        cb(err);
                    },
                        `CREATE TABLE ${tblName}( a datetime )`
                    )
                },

                (cb) => {
                    con.executeQuery(
                        (res, err) => {
                            cb(err);
                        },
                        `INSERT INTO ${tblName}(a)VALUES(?)`, date
                    );
                },

                (cb) => {
                    con.executeQuery(
                        (res, err) => {
                            assert.ok(res.a.getFullYear() == date.getFullYear());
                            assert.ok(res.a.getMonth() == date.getMonth());
                            assert.ok(res.a.getDate() == date.getDate());
                            assert.ok(res.a.getHours() == date.getHours());
                            assert.ok(res.a.getMinutes() == date.getMinutes());
                            assert.ok(res.a.getSeconds() == date.getSeconds());
                            assert.ok(res.a.getMilliseconds() == date.getMilliseconds());

                            cb(err);
                        },
                        `SELECT * FROM ${tblName}`
                    );
                }
            ], (err) => {
                if (err) {
                    done(err);
                    return;
                }
                done();
            });
        });

        it("insert numeric -> fetch numeric", (done) => {
            let tblName = mod.getRandomTableName();

            let con = new odbc.Connection()
                .connect(connection.connectionString);

            let num = odbc.makeNumericValue(18, 0, true, new Uint8Array([25, 20, 30, 40]));

            async.series([
                (cb) => {
                    con.executeQuery((res, err) => {
                        cb(err);
                    },
                        `CREATE TABLE ${tblName}( a numeric(18, 0) )`
                    )
                },

                (cb) => {
                    con.executeQuery(
                        (res, err) => {
                            cb(err);
                        },
                        `INSERT INTO ${tblName}(a)VALUES(?)`, num
                    );
                },

                (cb) => {
                    con.executeQuery<{ a: odbc.SqlNumeric }>(
                        (res, err) => {
                            if (res.a == undefined) {
                                throw "failed";
                            }

                            assert.ok(res.a.precision == num.precision, "invalid precision");
                            assert.ok(res.a.scale == num.scale, "invalid scale");
                            assert.ok(res.a.sign == num.sign, "invalid sign");

                            for (var i = 0; i < res.a.value.length; i++) {
                                if (i < num.value.length) {
                                    assert.ok(res.a.value[i] == num.value[i]);
                                }
                            }

                            cb(err);
                        },
                        `SELECT * FROM ${tblName}`
                    );
                }
            ], (err) => {
                if (err) {
                    done(err);
                    return;
                }
                done();
            });
        });

        //it("insert buffer -> fetch buffer", (done) => {
        //    let tblName = mod.getRandomTableName();

        //    let con = new odbc.Connection()
        //        .connect(connection.connectionString);

        //    let buf1 = Buffer.alloc(128);
        //    for (var i = 0; i < 128; i++) {
        //        buf1[i] = Math.floor(Math.random() * 127);
        //    }

        //    async.series([
        //        (cb) => {
        //            con.executeQuery((res, err) => {
        //                cb(err);
        //            },
        //                `CREATE TABLE ${tblName}( a varbinary(128) )`
        //            )
        //        },

        //        (cb) => {
        //            con.executeQuery(
        //                (res, err) => {
        //                    cb(err);
        //                },
        //                `INSERT INTO ${tblName}(a)VALUES(?)`, buf1
        //            );
        //        },

        //        (cb) => {
        //            con.executeQuery<{ a: Buffer }>(
        //                (res, err) => {
        //                    assert.ok(res.a.length == buf1.length);

        //                    for (var i = 0; i < res.a.length; i++) {
        //                        assert.ok(res.a[i] == buf1[i], `invalid symbol ${res.a[i]} : ${buf1[i]}`);
        //                    }
        //                    cb(err);
        //                },
        //                `SELECT * FROM ${tblName}`
        //            );
        //        }
        //    ], (err) => {
        //        if (err) {
        //            done(err);
        //        }
        //        done();
        //    });
        //});
    });

    describe(`query type checker insert / fetch (exec toArray) -> ${connection.name}`, function() {
        this.timeout(connection.timeout);

		        afterEach(() => {
            global.gc();
        });


        it("insert null -> fetch", (done) => {
            let tblName = mod.getRandomTableName();

            let con = new odbc.Connection()
                .connect(connection.connectionString);


            interface INumberInsert {
                a: number|null;
                b: number | null;
                c: number | null;
                d: number | null;
            }

            let arr = new Array<INumberInsert>();

            async.series([
                (cb) => {
                    con.executeQuery(
                        (res, err) => {
                            cb(err);
                        },
                        `CREATE TABLE ${tblName}( a int, b int, c int, d int )`
                    )
                },

                (cb) => {
                    for (let i = 0; i < (Math.random() * 20) + 150; i++) {
                        arr.push({
                            a: null,
                            b: null,
                            c: null,
                            d: null,
                        });
                    }

                    async.eachSeries(arr, (elem, scb) => {
                        con.executeQuery(
                            (res, err) => {
                                if (err != null) {
                                    scb(err);
                                    return;
                                }

                                scb();
                            },
                            `INSERT INTO ${tblName}(a, b, c, d)VALUES(?, ?, ?, ?)`,
                            elem.a, elem.b, elem.c, elem.d
                        );
                    },
                        (err: odbc.SqlError) => {
                            if (err != null) {
                                throw new Error(err.message);
                            }
                            else {
                                cb();
                            }
                        }
                    );
                },
                (cb) => {
                    con.executeQuery<INumberInsert>(
                        odbc.eFetchMode.eArray,
                        (res, err) => {
                            if (err != null) {
                                cb(err);
                                return;
                            }

                            assert.ok(res.length == arr.length);

                            for (var _x = 0; _x < res.length; _x++) {
                                let _a = arr[_x];
                                let _r = res[_x];

                                assert.ok(res[_x].a == arr[_x].a);
                                assert.ok(res[_x].b == arr[_x].b);
                                assert.ok(res[_x].c == arr[_x].c);
                                assert.ok(res[_x].d == arr[_x].d);
                            }

                            cb();
                        },
                        `SELECT * FROM ${tblName}`
                    );

                }
            ],
                (err, res) => {
                    if (err) {
                        done(err);
                    }
                    done();
                }
            );
        });

        it("insert bool -> fetch", (done) => {
            let tblName = mod.getRandomTableName();

            let con = new odbc.Connection()
                .connect(connection.connectionString);


            interface INumberInsert {
                a: boolean;
                b: boolean;
                c: boolean;
                d: boolean;
            }

            let arr = new Array<INumberInsert>();

            async.series([
                (cb) => {
                    con.executeQuery(
                        (res, err) => {
                            cb(err);
                        },
                        `CREATE TABLE ${tblName}( a bit, b bit, c bit, d bit )`
                    )
                },

                (cb) => {
                    for (let i = 0; i < (Math.random() * 20) + 150; i++) {
                        arr.push({
                            a: Math.random() >= 0.5 ? true : false,
                            b: Math.random() >= 0.5 ? true : false,
                            c: Math.random() >= 0.5 ? true : false,
                            d: Math.random() >= 0.5 ? true : false,
                        });
                    }

                    async.eachSeries(arr, (elem, scb) => {
                        con.executeQuery(
                            (res, err) => {
                                if (err != null) {
                                    scb(err);
                                    return;
                                }

                                scb();
                            },
                            `INSERT INTO ${tblName}(a, b, c, d)VALUES(?, ?, ?, ?)`,
                            elem.a, elem.b, elem.c, elem.d
                        );
                    },
                        (err: odbc.SqlError) => {
                            if (err != null) {
                                throw new Error(err.message);
                            }
                            else {
                                cb();
                            }
                        }
                    );
                },
                (cb) => {
                    con.executeQuery<INumberInsert>(
                        odbc.eFetchMode.eArray,
                        (res, err) => {
                            if (err != null) {
                                cb(err);
                                return;
                            }

                            assert.ok(res.length == arr.length);

                            for (var _x = 0; _x < res.length; _x++) {
                                let _a = arr[_x];
                                let _r = res[_x];

                                assert.ok(res[_x].a == arr[_x].a);
                                assert.ok(res[_x].b == arr[_x].b);
                                assert.ok(res[_x].c == arr[_x].c);
                                assert.ok(res[_x].d == arr[_x].d);
                            }

                            cb();
                        },
                        `SELECT * FROM ${tblName}`
                    );

                }
            ],
                (err, res) => {
                    if (err) {
                        done(err);
                        return;
                    }
                    done();
                }
            );
        });

        it("insert int32 -> fetch", (done) => {
            let tblName = mod.getRandomTableName();

            let con = new odbc.Connection()
                .connect(connection.connectionString);


            interface INumberInsert {
                a: number;
                b: number;
                c: number;
                d: number;
            }

            let arr = new Array<INumberInsert>();

            async.series([
                (cb) => {
                    con.executeQuery(
                        (res, err) => {
                            cb(err);
                        },
                        `CREATE TABLE ${tblName}( a int, b int, c int, d int )`
                    )
                },

                (cb) => {
                    for (let i = 0; i < (Math.random() * 20) + 150; i++) {
                        arr.push({
                            a: mod.getRandomInt(250, 50),
                            b: mod.getRandomInt(350, 50),
                            c: mod.getRandomInt(650, 50),
                            d: mod.getRandomInt(250, 50),
                        });
                    }

                    async.eachSeries(arr, (elem, scb) => {
                        con.executeQuery(
                            (res, err) => {
                                if (err != null) {
                                    scb(err);
                                    return;
                                }

                                scb();
                            },
                            `INSERT INTO ${tblName}(a, b, c, d)VALUES(?, ?, ?, ?)`,
                            elem.a, elem.b, elem.c, elem.d
                        );
                    },
                        (err: odbc.SqlError) => {
                            if (err != null) {
                                throw new Error(err.message);
                            }
                            else {
                                cb();
                            }
                        }
                    );
                },
                (cb) => {
                    con.executeQuery<INumberInsert>(
                        odbc.eFetchMode.eArray,
                        (res, err) => {
                            if (err != null) {
                                cb(err);
                                return;
                            }

                            assert.ok(res.length == arr.length);

                            for (var _x = 0; _x < res.length; _x++) {
                                let _a = arr[_x];
                                let _r = res[_x];

                                assert.ok(res[_x].a == arr[_x].a);
                                assert.ok(res[_x].b == arr[_x].b);
                                assert.ok(res[_x].c == arr[_x].c);
                                assert.ok(res[_x].d == arr[_x].d);
                            }

                            cb();
                        },
                        `SELECT * FROM ${tblName}`
                    );

                }
            ],
                (err, res) => {
                    if (err) {
                        done(err);
                    }
                    done();
                }
            );
        });

        it("insert int64 -> fetch", (done) => {
            let tblName = mod.getRandomTableName();

            let con = new odbc.Connection()
                .connect(connection.connectionString);


            interface INumberInsert {
                a: number;
                b: number;
                c: number;
                d: number;
            }

            let arr = new Array<INumberInsert>();

            async.series([
                (cb) => {
                    con.executeQuery(
                        (res, err) => {
                            cb(err);
                        },
                        `CREATE TABLE ${tblName}( a bigint, b bigint, c bigint, d bigint )`
                    )
                },

                (cb) => {
                    for (let i = 0; i < (Math.random() * 20) + 150; i++) {
                        arr.push({
                            a: mod.getRandomInt(4200000000, 2147483647),
                            b: mod.getRandomInt(4200000000, 2147483647),
                            c: mod.getRandomInt(4200000000, 2147483647),
                            d: mod.getRandomInt(4200000000, 2147483647),
                        });
                    }

                    async.eachSeries(arr, (elem, scb) => {
                        con.executeQuery(
                            (res, err) => {
                                if (err != null) {
                                    scb(err);
                                    return;
                                }

                                scb();
                            },
                            `INSERT INTO ${tblName}(a, b, c, d)VALUES(?, ?, ?, ?)`,
                            elem.a, elem.b, elem.c, elem.d
                        );
                    },
                        (err: odbc.SqlError) => {
                            if (err != null) {
                                throw new Error(err.message);
                            }
                            else {
                                cb();
                            }
                        }
                    );
                },
                (cb) => {
                    con.executeQuery<INumberInsert>(
                        odbc.eFetchMode.eArray,
                        (res, err) => {
                            if (err != null) {
                                cb(err);
                                return;
                            }

                            assert.ok(res.length == arr.length);

                            for (var _x = 0; _x < res.length; _x++) {
                                let _a = arr[_x];
                                let _r = res[_x];

                                assert.ok(res[_x].a == arr[_x].a);
                                assert.ok(res[_x].b == arr[_x].b);
                                assert.ok(res[_x].c == arr[_x].c);
                                assert.ok(res[_x].d == arr[_x].d);
                            }

                            cb();
                        },
                        `SELECT * FROM ${tblName}`
                    );

                }
            ],
                (err, res) => {
                    if (err) {
                        done(err);
                        return;
                    }
                    done();
                }
            );
        });

        it("insert double -> fetch", (done) => {
            let tblName = mod.getRandomTableName();

            let con = new odbc.Connection()
                .connect(connection.connectionString);


            interface INumberInsert {
                a: number;
                b: number;
                c: number;
                d: number;
            }

            let arr = new Array<INumberInsert>();

            async.series([
                (cb) => {
                    con.executeQuery(
                        (res, err) => {
                            cb(err);
                        },
                        `CREATE TABLE ${tblName}( a float, b float, c float, d float)`
                    )
                },

                (cb) => {
                    for (let i = 0; i < (Math.random() * 20) + 150; i++) {
                        arr.push({
                            a: Math.random() * 25000,
                            b: Math.random() * 25000,
                            c: Math.random() * 25000,
                            d: Math.random() * 25000,
                        });
                    }

                    async.eachSeries(arr, (elem, scb) => {
                        con.executeQuery(
                            (res, err) => {
                                if (err != null) {
                                    scb(err);
                                    return;
                                }

                                scb();
                            },
                            `INSERT INTO ${tblName}(a, b, c, d)VALUES(?, ?, ?, ?)`,
                            elem.a, elem.b, elem.c, elem.d
                        );
                    },
                        (err: odbc.SqlError) => {
                            if (err != null) {
                                throw new Error(err.message);
                            }
                            else {
                                cb();
                            }
                        }
                    );
                },
                (cb) => {
                    con.executeQuery<INumberInsert>(
                        odbc.eFetchMode.eArray,
                        (res, err) => {
                            if (err != null) {
                                cb(err);
                                return;
                            }
                            assert.ok(res.length == arr.length);

                            for (var _x = 0; _x < res.length; _x++) {
                                let _a = arr[_x];
                                let _r = res[_x];

                                assert.ok(res[_x].a == arr[_x].a);
                                assert.ok(res[_x].b == arr[_x].b);
                                assert.ok(res[_x].c == arr[_x].c);
                                assert.ok(res[_x].d == arr[_x].d);
                            }

                            cb();
                        },
                        `SELECT * FROM ${tblName}`
                    );

                }
            ],
                (err, res) => {
                    if (err) {
                        done(err);
                        return;
                    }
                    done();
                }
            );
        });

		//>#TODO: update string compare (else v8 crashes here)
        //it("insert string -> fetch (char)", (done) => {
        //    let tblName = mod.getRandomTableName();

        //    console.log(tblName);

        //    let con = new odbc.Connection()
        //        .connect(connection.connectionString);


        //    interface INumberInsert {
        //        a: string;
        //        b: string;
        //        c: string;
        //        d: string;
        //    }

        //    let arr = new Array<INumberInsert>();

        //    async.series([
        //        (cb) => {
        //            con.executeQuery(
        //                (res, err) => {
        //                    cb(err);
        //                },
        //                `CREATE TABLE ${tblName}( a char( 128 ), b char( 128 ), c char( 128 ), d char( 128 ))`
        //            )
        //        },

        //        (cb) => {
        //            for (let i = 0; i < (Math.random() * 20) + 150; i++) {
        //                arr.push({
        //                    a: mod.getRandomString(mod.getRandomInt(100, 25)),
        //                    b: mod.getRandomString(mod.getRandomInt(100, 25)),
        //                    c: mod.getRandomString(mod.getRandomInt(100, 25)),
        //                    d: mod.getRandomString(mod.getRandomInt(100, 25)),
        //                });
        //            }

        //            async.eachSeries(arr, (elem, scb) => {
        //                con.executeQuery(
        //                    (res, err) => {
        //                        if (err != null) {
        //                            scb(err);
        //                            return;
        //                        }

        //                        scb();
        //                    },
        //                    `INSERT INTO ${tblName}(a, b, c, d)VALUES(?, ?, ?, ?)`,
        //                    elem.a, elem.b, elem.c, elem.d
        //                );
        //            },
        //                (err: odbc.SqlError) => {
        //                    if (err != null) {
        //                        throw new Error(err.message);
        //                    }
        //                    else {
        //                        cb();
        //                    }
        //                }
        //            );
        //        },
        //        (cb) => {
        //            con.executeQuery<INumberInsert>(
        //                odbc.eFetchMode.eArray,
        //                (res, err) => {
        //                    if (err != null) {
        //                        cb(err);
        //                        return;
        //                    }
        //                    assert.ok(res.length == arr.length);

        //                    for (var _x = 0; _x < res.length; _x++) {

        //                        let _a = arr[_x];
        //                        let _r = res[_x];

        //                        var check = (a: string, b: string) => {
        //                            for (var i = 0; i < 128; i++) {
        //                                let rc = a.charCodeAt(i);
        //                                let c;
        //                                if (i >= b.length) {
        //                                    c = " ".charCodeAt(0);
        //                                }
        //                                else {
        //                                    c = b.charCodeAt(i);
        //                                }
        //                                assert(rc == c, `invalid char cod ${rc} : ${c}`);
        //                            }
        //                        };

        //                        check(_r.a, _a.a);
        //                        check(_r.b, _a.b);
        //                        check(_r.c, _a.c);
        //                        check(_r.d, _a.d);
        //                    }

        //                    cb();
        //                },
        //                `SELECT * FROM ${tblName}`
        //            );

        //        }
        //    ],
        //        (err, res) => {
        //            if (err) {
        //                done(err);
        //                return;
        //            }
        //            done();
        //        }
        //    );
        //});

        //it("insert string -> fetch (nchar)", (done) => {
        //    let tblName = mod.getRandomTableName();

        //    console.log(tblName);

        //    let con = new odbc.Connection()
        //        .connect(connection.connectionString);


        //    interface INumberInsert {
        //        a: string;
        //        b: string;
        //        c: string;
        //        d: string;
        //    }

        //    let arr = new Array<INumberInsert>();

        //    async.series([
        //        (cb) => {
        //            con.executeQuery(
        //                (res, err) => {
        //                    cb(err);
        //                },
        //                `CREATE TABLE ${tblName}( a nchar( 128 ), b nchar( 128 ), c nchar( 128 ), d nchar( 128 ))`
        //            )
        //        },

        //        (cb) => {
        //            for (let i = 0; i < (Math.random() * 20) + 150; i++) {
        //                arr.push({
        //                    a: mod.getRandomString(mod.getRandomInt(100, 25)),
        //                    b: mod.getRandomString(mod.getRandomInt(100, 25)),
        //                    c: mod.getRandomString(mod.getRandomInt(100, 25)),
        //                    d: mod.getRandomString(mod.getRandomInt(100, 25)),
        //                });
        //            }

        //            async.eachSeries(arr, (elem, scb) => {
        //                con.executeQuery(
        //                    (res, err) => {
        //                        if (err != null) {
        //                            scb(err);
        //                            return;
        //                        }

        //                        scb();
        //                    },
        //                    `INSERT INTO ${tblName}(a, b, c, d)VALUES(?, ?, ?, ?)`,
        //                    elem.a, elem.b, elem.c, elem.d
        //                );
        //            },
        //                (err: odbc.SqlError) => {
        //                    if (err != null) {
        //                        throw new Error(err.message);
        //                    }
        //                    else {
        //                        cb();
        //                    }
        //                }
        //            );
        //        },
        //        (cb) => {
        //            con.executeQuery<INumberInsert>(
        //                odbc.eFetchMode.eArray,
        //                (res, err) => {
        //                    if (err != null) {
        //                        cb(err);
        //                        return;
        //                    }
        //                    assert.ok(res.length == arr.length);

        //                    for (var _x = 0; _x < res.length; _x++) {

        //                        let _a = arr[_x];
        //                        let _r = res[_x];

        //                        var check = (a: string, b: string) => {
        //                            for (var i = 0; i < 128; i++) {
        //                                let rc = a.charCodeAt(i);
        //                                let c;
        //                                if (i >= b.length) {
        //                                    c = " ".charCodeAt(0);
        //                                }
        //                                else {
        //                                    c = b.charCodeAt(i);
        //                                }
        //                                assert(rc == c, `invalid char cod ${rc} : ${c}`);
        //                            }
        //                        };

        //                        check(_r.a, _a.a);
        //                        check(_r.b, _a.b);
        //                        check(_r.c, _a.c);
        //                        check(_r.d, _a.d);
        //                    }

        //                    cb();
        //                },
        //                `SELECT * FROM ${tblName}`
        //            );

        //        }
        //    ],
        //        (err, res) => {
        //            if (err) {
        //                done(err);
        //                return;
        //            }
        //            done();
        //        }
        //    );
        //});

        //it("insert string -> fetch (varchar)", (done) => {
        //    let tblName = mod.getRandomTableName();

        //    console.log(tblName);

        //    let con = new odbc.Connection()
        //        .connect(connection.connectionString);


        //    interface INumberInsert {
        //        a: string;
        //        b: string;
        //        c: string;
        //        d: string;
        //    }

        //    let arr = new Array<INumberInsert>();

        //    async.series([
        //        (cb) => {
        //            con.executeQuery(
        //                (res, err) => {
        //                    cb(err);
        //                },
        //                `CREATE TABLE ${tblName}( a varchar( 128 ), b varchar( 128 ), c varchar( 128 ), d varchar( 128 ))`
        //            )
        //        },

        //        (cb) => {
        //            for (let i = 0; i < (Math.random() * 20) + 150; i++) {
        //                arr.push({
        //                    a: mod.getRandomString(mod.getRandomInt(100, 25)),
        //                    b: mod.getRandomString(mod.getRandomInt(100, 25)),
        //                    c: mod.getRandomString(mod.getRandomInt(100, 25)),
        //                    d: mod.getRandomString(mod.getRandomInt(100, 25)),
        //                });
        //            }

        //            async.eachSeries(arr, (elem, scb) => {
        //                con.executeQuery(
        //                    (res, err) => {
        //                        if (err != null) {
        //                            scb(err);
        //                            return;
        //                        }

        //                        scb();
        //                    },
        //                    `INSERT INTO ${tblName}(a, b, c, d)VALUES(?, ?, ?, ?)`,
        //                    elem.a, elem.b, elem.c, elem.d
        //                );
        //            },
        //                (err: odbc.SqlError) => {
        //                    if (err != null) {
        //                        throw new Error(err.message);
        //                    }
        //                    else {
        //                        cb();
        //                    }
        //                }
        //            );
        //        },
        //        (cb) => {
        //            con.executeQuery<INumberInsert>(
        //                odbc.eFetchMode.eArray,
        //                (res, err) => {
        //                    if (err != null) {
        //                        cb(err);
        //                        return;
        //                    }
        //                    assert.ok(res.length == arr.length);

        //                    for (var _x = 0; _x < res.length; _x++) {

        //                        let _a = arr[_x];
        //                        let _r = res[_x];

        //                        var check = (a: string, b: string) => {
        //                            for (var i = 0; i < 128; i++) {
        //                                let rc = a.charCodeAt(i);
        //                                let c;
        //                                if (i >= b.length) {
        //                                    c = " ".charCodeAt(0);
        //                                }
        //                                else {
        //                                    c = b.charCodeAt(i);
        //                                }
        //                                assert(rc == c, `invalid char cod ${rc} : ${c}`);
        //                            }
        //                        };

        //                        check(_r.a, _a.a);
        //                        check(_r.b, _a.b);
        //                        check(_r.c, _a.c);
        //                        check(_r.d, _a.d);
        //                    }

        //                    cb();
        //                },
        //                `SELECT * FROM ${tblName}`
        //            );

        //        }
        //    ],
        //        (err, res) => {
        //            if (err) {
        //                done(err);
        //                return;
        //            }
        //            done();
        //        }
        //    );
        //});

        //it("insert string -> fetch (nvarchar)", (done) => {
        //    let tblName = mod.getRandomTableName();

        //    console.log(tblName);

        //    let con = new odbc.Connection()
        //        .connect(connection.connectionString);


        //    interface INumberInsert {
        //        a: string;
        //        b: string;
        //        c: string;
        //        d: string;
        //    }

        //    let arr = new Array<INumberInsert>();

        //    async.series([
        //        (cb) => {
        //            con.executeQuery(
        //                (res, err) => {
        //                    cb(err);
        //                },
        //                `CREATE TABLE ${tblName}( a nvarchar( 128 ), b nvarchar( 128 ), c nvarchar( 128 ), d nvarchar( 128 ))`
        //            )
        //        },

        //        (cb) => {
        //            for (let i = 0; i < (Math.random() * 20) + 150; i++) {
        //                arr.push({
        //                    a: mod.getRandomString(mod.getRandomInt(100, 25)),
        //                    b: mod.getRandomString(mod.getRandomInt(100, 25)),
        //                    c: mod.getRandomString(mod.getRandomInt(100, 25)),
        //                    d: mod.getRandomString(mod.getRandomInt(100, 25)),
        //                });
        //            }

        //            async.eachSeries(arr, (elem, scb) => {
        //                con.executeQuery(
        //                    (res, err) => {
        //                        if (err != null) {
        //                            scb(err);
        //                            return;
        //                        }

        //                        scb();
        //                    },
        //                    `INSERT INTO ${tblName}(a, b, c, d)VALUES(?, ?, ?, ?)`,
        //                    elem.a, elem.b, elem.c, elem.d
        //                );
        //            },
        //                (err: odbc.SqlError) => {
        //                    if (err != null) {
        //                        throw new Error(err.message);
        //                    }
        //                    else {
        //                        cb();
        //                    }
        //                }
        //            );
        //        },
        //        (cb) => {
        //            con.executeQuery<INumberInsert>(
        //                odbc.eFetchMode.eArray,
        //                (res, err) => {
        //                    if (err != null) {
        //                        cb(err);
        //                        return;
        //                    }
        //                    assert.ok(res.length == arr.length);

        //                    for (var _x = 0; _x < res.length; _x++) {

        //                        let _a = arr[_x];
        //                        let _r = res[_x];

        //                        var check = (a: string, b: string) => {
        //                            for (var i = 0; i < 128; i++) {
        //                                let rc = a.charCodeAt(i);
        //                                let c;
        //                                if (i >= b.length) {
        //                                    c = " ".charCodeAt(0);
        //                                }
        //                                else {
        //                                    c = b.charCodeAt(i);
        //                                }
        //                                assert(rc == c, `invalid char cod ${rc} : ${c}`);
        //                            }
        //                        };

        //                        check(_r.a, _a.a);
        //                        check(_r.b, _a.b);
        //                        check(_r.c, _a.c);
        //                        check(_r.d, _a.d);
        //                    }

        //                    cb();
        //                },
        //                `SELECT * FROM ${tblName}`
        //            );

        //        }
        //    ],
        //        (err, res) => {
        //            if (err) {
        //                done(err);
        //                return;
        //            }
        //            done();
        //        }
        //    );
        //});

    });


});

