import * as assert from "assert";
import * as odbc from "../lib/node-odbc";
import * as mod from "./module";
import * as async from "async";


mod.connection.forEach((connection) => {
    describe(`query type checker -> promise ${connection.name} (toSingle)`, function() {
        this.timeout(connection.timeout + 2500 );

        it("insert null -> fetch null", (done) => {
            let tblName = mod.getRandomTableName();

            let con = new odbc.Connection()
                .connect(connection.connectionString);

            let r = async () => {
                await con.prepareQuery(`CREATE TABLE ${tblName}( a int )`).toSingle();
                await con.prepareQuery(`INSERT INTO ${tblName}(a)VALUES(?)`, null).toSingle();
                let r = await con.prepareQuery(`SELECT * FROM ${tblName}`).toSingle<{ a: number }>();

                assert.ok(r.a == null);
            };

            r().then(() => {
                done();
            }).catch((err) => {
                done(err);
            });
        });

        it("insert bool -> fetch bool", (done) => {
            let tblName = mod.getRandomTableName();

            let con = new odbc.Connection()
                .connect(connection.connectionString);


            let val = Math.random() > 0.5 ? true : false;

            let r = async () => {
                await con.prepareQuery(`CREATE TABLE ${tblName}( a bit )`).toSingle();
                await con.prepareQuery(`INSERT INTO ${tblName}(a)VALUES(?)`, val).toSingle();
                let r = await con.prepareQuery(`SELECT * FROM ${tblName}`).toSingle<{ a: boolean }>();

                assert.ok(r.a == val);
            };

            r().then(() => {
                done();
            }).catch((err) => {
                done(err);
            });
        });

        it("insert int32 -> fetch int32", (done) => {
            let tblName = mod.getRandomTableName();

            let con = new odbc.Connection()
                .connect(connection.connectionString);

            let num = mod.getRandomInt(250, 50);

            let r = async () => {
                await con.prepareQuery(`CREATE TABLE ${tblName}( a int )`).toSingle();
                await con.prepareQuery(`INSERT INTO ${tblName}(a)VALUES(?)`, num).toSingle();
                let r = await con.prepareQuery(`SELECT * FROM ${tblName}`).toSingle<{ a: number }>();

                assert.ok(r.a == num);
            };

            r().then(() => {
                done();
            }).catch((err) => {
                done(err);
            });
        });

        it("insert int64 -> fetch int32", (done) => {
            let tblName = mod.getRandomTableName();

            let con = new odbc.Connection()
                .connect(connection.connectionString);

            let num = mod.getRandomInt(4200000000, 2147483647);

            let r = async () => {
                await con.prepareQuery(`CREATE TABLE ${tblName}( a bigint )`).toSingle();
                await con.prepareQuery(`INSERT INTO ${tblName}(a)VALUES(?)`, num).toSingle();
                let r = await con.prepareQuery(`SELECT * FROM ${tblName}`).toSingle<{ a: number }>();

                assert.ok(r.a == num);
            };

            r().then(() => {
                done();
            }).catch((err) => {
                done(err);
            });
        });

        it("insert double -> fetch double", (done) => {
            let tblName = mod.getRandomTableName();

            let con = new odbc.Connection()
                .connect(connection.connectionString);

            let num = Math.random() * 250;

            let r = async () => {
                await con.prepareQuery(`CREATE TABLE ${tblName}( a float )`).toSingle();
                await con.prepareQuery(`INSERT INTO ${tblName}(a)VALUES(?)`, num).toSingle();
                let r = await con.prepareQuery(`SELECT * FROM ${tblName}`).toSingle<{ a: number }>();

                assert.ok(r.a == num);
            };

            r().then(() => {
                done();
            }).catch((err) => {
                done(err);
            });
        });

        it("insert string -> fetch string (char)", (done) => {
            let tblName = mod.getRandomTableName();

            let con = new odbc.Connection()
                .connect(connection.connectionString);

            let str = mod.getRandomString( 100 );

            let r = async () => {
                await con.prepareQuery(`CREATE TABLE ${tblName}( a char(128) )`).toSingle();
                await con.prepareQuery(`INSERT INTO ${tblName}(a)VALUES(?)`, str).toSingle();
                let r = await con.prepareQuery(`SELECT * FROM ${tblName}`).toSingle<{ a: string }>();

                for (var i = 0; i < 128; i++) {
                    if (r.a == undefined) {
                        throw "failed";
                    }


                    let rc = r.a.charCodeAt(i);
                    let c;
                    if (i >= str.length) {
                        c = " ".charCodeAt(0);
                    }
                    else {
                        c = str.charCodeAt(i);
                    }
                    assert(rc == c);
                }
            };

            r().then(() => {
                done();
            }).catch((err) => {
                done(err);
            });
        });

        it("insert string -> fetch string (nchar)", (done) => {
            let tblName = mod.getRandomTableName();

            let con = new odbc.Connection()
                .connect(connection.connectionString);

            let str = mod.getRandomString(100);

            let r = async () => {
                await con.prepareQuery(`CREATE TABLE ${tblName}( a nchar(128) )`).toSingle();
                await con.prepareQuery(`INSERT INTO ${tblName}(a)VALUES(?)`, str).toSingle();
                let r = await con.prepareQuery(`SELECT * FROM ${tblName}`).toSingle<{ a: string }>();

                for (var i = 0; i < 128; i++) {
                    if (r.a == undefined) {
                        throw "failed";
                    }

                    let rc = r.a.charCodeAt(i);
                    let c;
                    if (i >= str.length) {
                        c = " ".charCodeAt(0);
                    }
                    else {
                        c = str.charCodeAt(i);
                    }
                    assert(rc == c);
                }
            };

            r().then(() => {
                done();
            }).catch((err) => {
                done(err);
            });
        });

        it("insert string -> fetch string (varchar)", (done) => {
            let tblName = mod.getRandomTableName();

            let con = new odbc.Connection()
                .connect(connection.connectionString);

            let str = mod.getRandomString(100);

            let r = async () => {
                await con.prepareQuery(`CREATE TABLE ${tblName}( a varchar(128) )`).toSingle();
                await con.prepareQuery(`INSERT INTO ${tblName}(a)VALUES(?)`, str).toSingle();
                let r = await con.prepareQuery(`SELECT * FROM ${tblName}`).toSingle<{ a: string }>();

				if( r.a == undefined )
				{
					throw "failed";
				}

				assert.equal( str.length, r.a.length ); 
                assert(r.a == str, `invalid string ${r.a} : ${str}`);
            };

            r().then(() => {
                done();
            }).catch((err) => {
                done(err);
            });
        });

        it("insert string -> fetch string (nvarchar)", (done) => {
            let tblName = mod.getRandomTableName();

            let con = new odbc.Connection()
                .connect(connection.connectionString);

            let str = mod.getRandomString(100);

            let r = async () => {
                await con.prepareQuery(`CREATE TABLE ${tblName}( a nvarchar(128) )`).toSingle();
                await con.prepareQuery(`INSERT INTO ${tblName}(a)VALUES(?)`, str).toSingle();
                let r = await con.prepareQuery(`SELECT * FROM ${tblName}`).toSingle<{ a: string }>();

                assert(r.a == str);
            };

            r().then(() => {
                done();
            }).catch((err) => {
                done(err);
            });
        });
	
        it("insert timestamp -> fetch date", (done) => {
            let tblName = mod.getRandomTableName();

            let con = new odbc.Connection()
				.connect( connection.connectionString );

			let date = new Date( 2017, 6, 7, 5, 46, 12, 623 );

            let r = async () => {
				await con.prepareQuery( `CREATE TABLE ${tblName}( a datetime )` ).toSingle();
				await con.prepareQuery( `INSERT INTO ${tblName}(a)VALUES(?)`, odbc.makeTimestamp( date ) ).toSingle();
                let r = await con.prepareQuery(`SELECT * FROM ${tblName}`).toSingle<{ a: Date }>();

				console.log( r );
				console.log( date );

                if (r.a == undefined) {
                    throw "failed";
				}

				assert.ok( r.a.getFullYear() == date.getFullYear() );
				assert.ok( r.a.getMonth() == date.getMonth() );
				assert.ok( r.a.getDate() == date.getDate() );
				assert.ok( r.a.getHours() == date.getHours() );
				assert.ok( r.a.getMinutes() == date.getMinutes() );
				assert.ok( r.a.getSeconds() == date.getSeconds() );
				assert.ok( r.a.getMilliseconds() == date.getMilliseconds() );
            };

            r().then(() => {
                done();
            }).catch((err) => {
                done(err);
            });
        });

        it("insert date -> fetch date", (done) => {
            let tblName = mod.getRandomTableName();

            let con = new odbc.Connection()
                .connect(connection.connectionString);

			let date = new Date( Date.now() );

            let r = async () => {
                await con.prepareQuery(`CREATE TABLE ${tblName}( a datetime )`).toSingle();
                await con.prepareQuery(`INSERT INTO ${tblName}(a)VALUES(?)`, date).toSingle();
                let r = await con.prepareQuery(`SELECT * FROM ${tblName}`).toSingle<{ a: Date }>();

                if (r.a == undefined) {
                    throw "failed";
				}

				assert( r.a.getFullYear() == date.getFullYear() );
				assert( r.a.getMonth() == date.getMonth() );
				assert( r.a.getDate() == date.getDate() );

            };

            r().then(() => {
                done();
            }).catch((err) => {
                done(err);
            });
        });

        it("insert numeric -> fetch numeric", (done) => {
            let tblName = mod.getRandomTableName();

            let con = new odbc.Connection()
                .connect(connection.connectionString);

			let num = odbc.makeNumeric( 18, 0, true, new Uint8Array( [25, 20, 30, 40] ) );

            let r = async () => {
                await con.prepareQuery(`CREATE TABLE ${tblName}( a numeric(18, 0) )`).toSingle();
                await con.prepareQuery(`INSERT INTO ${tblName}(a)VALUES(?)`, num).toSingle();
                let r = await con.prepareQuery(`SELECT * FROM ${tblName}`).toSingle<{ a: odbc.SqlNumeric }>();

                if (r.a == undefined) {
                    throw "failed";
                }
                assert.ok(r.a.precision == num.precision, "invalid precision");
                assert.ok(r.a.scale == num.scale, "invalid scale");
                assert.ok(r.a.sign == num.sign, "invalid sign");

                for (var i = 0; i < r.a.value.length; i++) {
                    if (i < num.value.length) {
                        assert.ok(r.a.value[i] == num.value[i]);
                    }
                }
            };

            r().then(() => {
                done();
            }).catch((err) => {
                done(err);
            });
        });

        //it("insert buffer -> fetch buffer", (done) => {
        //    let tblName = mod.getRandomTableName();

        //    let con = new odbc.Connection()
        //        .connect(connection.connectionString);

        //    let buf = Buffer.alloc(128);
        //    for (var i = 0; i < 128; i++) {
        //        buf[i] = Math.floor(Math.random() * 127);
        //    }

        //    let r = async () => {
        //        await con.prepareQuery(`CREATE TABLE ${tblName}( a varbinary(128) )`).toSingle();
        //        await con.prepareQuery(`INSERT INTO ${tblName}(a)VALUES(?)`, buf).toSingle();
        //        let r = await con.prepareQuery(`SELECT * FROM ${tblName}`).toSingle<{ a: Buffer }>();
        //        assert.ok(r.a.length == buf.length);

        //        for (var i = 0; i < r.a.length; i++) {
        //            assert.ok(r.a[i] == buf[i], `invalid symbol ${r.a[i]} : ${buf[i]}`);
        //        }
        //    };

        //    r().then(() => {
        //        done();
        //    }).catch((err) => {
        //        done(err);
        //    });
        //});
    });

	//> todo create nullable tests with all data types
    describe(`query type checker -> promise ${connection.name} (toArray)`, function() {
        this.timeout(connection.timeout);

        it("insert null -> fetch null", (done) => {
            let tblName = mod.getRandomTableName();

            let con = new odbc.Connection()
                .connect(connection.connectionString);

            let val = new Array< number | null >();

            for (let i = 0; i < mod.getRandomInt(50, 15); i++) {
                val.push(null);
            }

            let r = async () => {
                await con.prepareQuery(`CREATE TABLE ${tblName}( a int )`).toSingle();

                for (let i of val) {
                    await con.prepareQuery(`INSERT INTO ${tblName}(a)VALUES(?)`, null).toSingle();
                }

                let r = await con.prepareQuery(`SELECT * FROM ${tblName}`).toArray<{ a: number }>();

                assert.ok(r.length == val.length);

                for (let i = 0; i < r.length; i++){
                    assert.ok(r[i].a == val[i]);
                }
            };

            r().then(() => {
                done();
            }).catch((err) => {
                done(err);
            });
        });

        it("insert bool -> fetch bool", (done) => {
            let tblName = mod.getRandomTableName();

            let con = new odbc.Connection()
                .connect(connection.connectionString);

            let val = new Array< boolean >();

            for (let i = 0; i < mod.getRandomInt(50, 15); i++) {
                val.push(Math.random() >= 0.5 ? true : false);
            }

            let r = async () => {
                await con.prepareQuery(`CREATE TABLE ${tblName}( a bit )`).toSingle();

                for (let i of val) {
                    await con.prepareQuery(`INSERT INTO ${tblName}(a)VALUES(?)`, i).toSingle();
                }

                let r = await con.prepareQuery(`SELECT * FROM ${tblName}`).toArray<{ a: boolean }>();

                assert.ok(r.length == val.length);

                for (let i = 0; i < r.length; i++) {
                    assert.ok(r[i].a == val[i], `invalid type: ${r[i]} ${val[i]}`);
                }
            };

            r().then(() => {
                done();
            }).catch((err) => {
                done(err);
            });
        });

        it("insert int32 -> fetch int32", (done) => {
            let tblName = mod.getRandomTableName();

            let con = new odbc.Connection()
                .connect(connection.connectionString);

            let val: number[] = [];

            for (let i = 0; i < mod.getRandomInt(50, 15); i++) {
                val.push( mod.getRandomInt( 300, 500 ));
            }

            let r = async () => {
                await con.prepareQuery(`CREATE TABLE ${tblName}( a int )`).toSingle();

                for (let i of val) {
                    await con.prepareQuery(`INSERT INTO ${tblName}(a)VALUES(?)`, i).toSingle();
                }

                let r = await con.prepareQuery(`SELECT * FROM ${tblName}`).toArray<{ a: number }>();

                assert.ok(r.length == val.length);

                for (let i = 0; i < r.length; i++) {
                    assert.ok(r[i].a == val[i]);
                }
            };

            r().then(() => {
                done();
            }).catch((err) => {
                done(err);
            });
        });

        it("insert int64 -> fetch int64", (done) => {
            let tblName = mod.getRandomTableName();

            let con = new odbc.Connection()
                .connect(connection.connectionString);

            let val: number[] = [];

            for (let i = 0; i < mod.getRandomInt(50, 15); i++) {
                val.push(mod.getRandomInt(4200000000, 2147483647));
            }

            let r = async () => {
                await con.prepareQuery(`CREATE TABLE ${tblName}( a bigint )`).toSingle();

                for (let i of val) {
                    await con.prepareQuery(`INSERT INTO ${tblName}(a)VALUES(?)`, i).toSingle();
                }

                let r = await con.prepareQuery(`SELECT * FROM ${tblName}`).toArray<{ a: number }>();

                assert.ok(r.length == val.length);

                for (let i = 0; i < r.length; i++) {
                    assert.ok(r[i].a == val[i]);
                }
            };

            r().then(() => {
                done();
            }).catch((err) => {
                done(err);
            });
        });

        it("insert double -> fetch double", (done) => {
            let tblName = mod.getRandomTableName();

            let con = new odbc.Connection()
                .connect(connection.connectionString);

            let val:number[] = [];

            for (let i = 0; i < mod.getRandomInt(50, 15); i++) {
                val.push(Math.random() * 250 );
            }

            let r = async () => {
                await con.prepareQuery(`CREATE TABLE ${tblName}( a float )`).toSingle();

                for (let i of val) {
                    await con.prepareQuery(`INSERT INTO ${tblName}(a)VALUES(?)`, i).toSingle();
                }

                let r = await con.prepareQuery(`SELECT * FROM ${tblName}`).toArray<{ a: number }>();

                assert.ok(r.length == val.length);

                for (let i = 0; i < r.length; i++) {
                    assert.ok(r[i].a == val[i]);
                }
            };

            r().then(() => {
                done();
            }).catch((err) => {
                done(err);
            });
        });

        it("insert string -> fetch string(varchar)", (done) => {
            let tblName = mod.getRandomTableName();

            let con = new odbc.Connection()
                .connect(connection.connectionString);

            let val:string[] = [];

            for (let i = 0; i < mod.getRandomInt(50, 15); i++) {
                val.push(mod.getRandomString( 120));
            }

            let r = async () => {
                await con.prepareQuery(`CREATE TABLE ${tblName}( a varchar(128) )`).toSingle();

                for (let i of val) {
                    await con.prepareQuery(`INSERT INTO ${tblName}(a)VALUES(?)`, i).toSingle();
                }

                let r = await con.prepareQuery(`SELECT * FROM ${tblName}`).toArray<{ a: string }>();

                assert.ok(r.length == val.length);

                for (let i = 0; i < r.length; i++) {
                    assert.ok(r[i].a == val[i]);
                }
            };

            r().then(() => {
                done();
            }).catch((err) => {
                done(err);
            });
        });

        it("insert string -> fetch string(nvarchar)", (done) => {
            let tblName = mod.getRandomTableName();

            let con = new odbc.Connection()
                .connect(connection.connectionString);

            let val:string[] = [];

            for (let i = 0; i < mod.getRandomInt(50, 15); i++) {
                val.push(mod.getRandomString(120));
            }

            let r = async () => {
                await con.prepareQuery(`CREATE TABLE ${tblName}( a nvarchar(128) )`).toSingle();

                for (let i of val) {
                    await con.prepareQuery(`INSERT INTO ${tblName}(a)VALUES(?)`, i).toSingle();
                }

                let r = await con.prepareQuery(`SELECT * FROM ${tblName}`).toArray<{ a: string }>();

                assert.ok(r.length == val.length);

                for (let i = 0; i < r.length; i++) {
                    assert.ok(r[i].a == val[i]);
                }
            };

            r().then(() => {
                done();
            }).catch((err) => {
                done(err);
            });
        });

    });

});