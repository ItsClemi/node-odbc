{
    'variables': {
    },

	'includes': [
      #'src/lib/tbb-2017_U5/tbb.gypi',
	  #'src/lib/tbb-2017_U5/tbbmalloc.gypi',
    ],


    "targets": [
        {
            "target_name": "node-odbc",

			'dependencies': [
			  #'xyzzy',
			  #'../bar/bar.gyp:bar',
			],

            "sources": [
                "src/stdafx.cpp",

                "src/Common/Helper/Utility.cpp",
                "src/Common/Logger/Log.cpp",
                "src/Common/Logger/LogImpl.cpp",

                "src/Core/GlobalEnvironment.cpp",

                "src/Export/EConnection.cpp",
                "src/Export/EModuleHelper.cpp",

                "src/Odbc/Helper/OdbcError.cpp",
                "src/Odbc/OdbcEnvironment.cpp",

                "src/Module.cpp"
            ],

         	  "include_dirs": [
                       "src",
                       ],


         	  'conditions': [
                       ['OS=="win"', {
                           'defines': [
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
