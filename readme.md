# node-odbc
### A simple odbc interface for nodejs

#### Note: node-odbc is in early development. There might be some bugs left!


##### Supported types

node-odbc auto maps your types to the specific odbc types.
* number -> int32, int64, double
* string -> char, nchar, varchar, nvarchar
* date -> datetime


##### Examples

Callback:

```javascript
let con = new odbc.Connection()
    .connect(connectionString);

// run query
con.executeQuery((res, err) => 
    {  
        if( err != null )
        {
            console.log( err );
        }
    },
    "CREATE TABLE tblTest( myColumn int )"
);

//insert data
con.executeQuery((res, err) => 
    { 
        if( err != null )
        {
            console.log( err );
        }
    },
    "INSERT INTO tblTest(myColumn)VALUES(?)", 250
);


//fetch data
con.executeQuery((res, err) => 
    {
        if( err != null )
        {
            console.log( err );
        }

        console.log( res.myColumn ); // == 250
    },
    "SELECT * FROM tblTest"
);
```

Promise:

```javascript
let con = new odbc.Connection()
    .connect(connection.connectionString);

let r = async () => {
    await con.prepareQuery("CREATE TABLE tblTest( myColumn int )").toSingle();
    await con.prepareQuery("INSERT INTO tblTest( myColumn )VALUES(?)", 250).toSingle();
    let r = await con.prepareQuery("SELECT * FROM tblTest").toSingle();

    console.log( r.myColumn ); // == 250
};

r().then(() => {

}).catch((err) => {
    console.log( err );
});
```

Check out the test folder to find more documentation.


### Roadmap

* Add Documentation
* Add support for Linux/Darwin
* Add node:buffer support
* Add LOB data support
* Test more data sources