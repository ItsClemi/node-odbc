{
    "targets": [
        {
            "win_delay_load_hook": "true",
            'default_configuration': 'Release',

            "target_name": "node-odbc",

            'dependencies': [
                "src/lib/tbb-2017_U5/binding.gyp:tbb",
                "src/lib/tbb-2017_U5/binding.gyp:tbbmalloc",
            ],

            "sources": [
                "src/stdafx.cpp",

                "src/Common/Helper/Utility.cpp",
                "src/Core/Connection/ConnectionTracker.cpp",
                "src/Core/Connection/QueryTracker.cpp",

                "src/Core/libuv/UvOperation.cpp",

                "src/Core/GlobalEnvironment.cpp",

                "src/Export/EConnection.cpp",
                "src/Export/EPreparedQuery.cpp",
                "src/Export/EModuleHelper.cpp",

                "src/Odbc/Connection/Extension/MssqlExtension.cpp",
                "src/Odbc/Connection/Extension/PoolExtension.cpp",
                "src/Odbc/Connection/ConnectionPool.cpp",

                "src/Odbc/Helper/OdbcError.cpp",
                "src/Odbc/Query/Query.cpp",
                "src/Odbc/Query/QueryParameter.cpp",
                "src/Odbc/Query/ResultSet.cpp",

                "src/Odbc/OdbcEnvironment.cpp",

                "src/Module.cpp"
            ],

            "include_dirs": [
                "<!(node -e \"require('nan')\")",

                "src",
                "src/lib/tbb-2017_U5/include",
            ],


            'conditions': [
                ['OS=="win"', {
                    'defines': [
                        'WIN32',
                        '_WINDOWS',
                        'NDEBUG',
                        'UNICODE=1',
                        '_UNICODE=1',
                        '_SQLNCLI_ODBC_',
                    ],
                }]
            ],


            'msvs_settings': {
                'VCLinkerTool': {
                    'SetChecksum': 'true'
                }
            },
        }
    ]
}
