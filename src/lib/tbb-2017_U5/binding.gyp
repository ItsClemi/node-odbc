{
    'variables': {
    },

    'targets': [
        {
            'target_name': 'tbbmalloc',
            'type': 'static_library',
            'default_configuration': 'Release',


            'defines': [
                '__TBBMALLOC_BUILD=1',
                '__TBB_NO_IMPLICIT_LINKAGE=1',
                '__TBBMALLOC_NO_IMPLICIT_LINKAGE=1',
                'DO_ITT_NOTIFY',
                '__TBB_SOURCE_DIRECTLY_INCLUDED',
            ],

            'include_dirs': [
                'include',
                'src/rml/include',
                'src',
            ],

            'sources': [
                'src/tbbmalloc/backend.cpp',
                'src/tbbmalloc/large_objects.cpp',
                'src/tbbmalloc/backref.cpp',
                'src/tbbmalloc/tbbmalloc.cpp',
                'src/tbbmalloc/frontend.cpp',
                'src/tbb/itt_notify.cpp',
            ],

            'conditions': [
                ['OS=="linux"', {
                    'defines': [
                        'LINUX_DEFINE',
                    ],
                    'include_dirs': [
                        'include/linux',
                    ],
                }],

                ['OS=="win"', {
                    'msvs_disabled_warnings': [
                        4267,
                        4800,
                        4146,
                        4244,
                        4577,
                        4018,
                    ],

                    'defines': [
                        '_CRT_SECURE_NO_DEPRECATE',
                        '_WIN32_WINNT=0x0600',
                        'USE_WINTHREAD',
                        'WIN32',
                        '_WINDOWS',
                        'NDEBUG',
                    ],
                }, {  # OS != "win",
                    'defines': [
                        'NON_WINDOWS_DEFINE',
                    ],
                }]
            ],

            'configurations': {
                'Debug': {
                    'defines': [
                        'DEBUG',
                        '_DEBUG',
                    ],

                    'cflags': ['-g', '-O0'],
                    'conditions': [
                        ['target_arch=="x64"', {
                            'msvs_configuration_platform': 'x64',
                        }],
                    ],
                    'msvs_settings': {
                        'VCCLCompilerTool': {
                            'Optimization': 0,  # /Od, no optimization
                            'MinimalRebuild': 'false',
                            'OmitFramePointers': 'false',
                            'BasicRuntimeChecks': 3,  # /RTC1
                            'RuntimeLibrary': '3',
                        },
                        'VCLinkerTool': {
                            'LinkIncremental': 2,  # enable incremental linking
                        },
                    },
                    'xcode_settings': {
                        'GCC_OPTIMIZATION_LEVEL': '0',  # stop gyp from defaulting to -Os
                    },
                },

                'Release': {
                    'defines': [
                        'NDEBUG',
                    ],

                    'msvs_settings':{
                        'VCCLCompilerTool': {
                            'WholeProgramOptimization': 'false',
                            'Optimization': '1',
                            'RuntimeLibrary': '2',
                        },
                        'VCLinkerTool': {
                            'LinkTimeCodeGeneration': '2',
                        }
                    }
                },
            },
        },

        {
            'target_name': 'tbb',

            'type': 'static_library',
            'default_configuration': 'Release',


            'defines': [
                '__TBB_BUILD=1',
                '__TBB_NO_IMPLICIT_LINKAGE=1',
                'DO_ITT_NOTIFY',
                '__TBB_SOURCE_DIRECTLY_INCLUDED',
                'TBB_PREVIEW_MEMORY_POOL',
            ],

            'include_dirs': [
                'include',
                'src/rml/include',
                'src',
            ],

            'sources': [
                'src/tbb/arena.cpp',
                'src/tbb/cache_aligned_allocator.cpp',
                'src/tbb/concurrent_hash_map.cpp',
                'src/tbb/concurrent_monitor.cpp',
                'src/tbb/concurrent_queue.cpp',
                'src/tbb/concurrent_vector.cpp',
                'src/tbb/condition_variable.cpp',
                'src/tbb/critical_section.cpp',
                'src/tbb/dynamic_link.cpp',
                'src/tbb/governor.cpp',
                'src/tbb/itt_notify.cpp',
                'src/tbb/market.cpp',
                'src/tbb/mutex.cpp',
                'src/tbb/observer_proxy.cpp',
                'src/tbb/pipeline.cpp',
                'src/tbb/private_server.cpp',
                'src/tbb/queuing_mutex.cpp',
                'src/tbb/queuing_rw_mutex.cpp',
                'src/tbb/reader_writer_lock.cpp',
                'src/tbb/recursive_mutex.cpp',
                'src/tbb/scheduler.cpp',
                'src/tbb/semaphore.cpp',
                'src/tbb/spin_mutex.cpp',
                'src/tbb/spin_rw_mutex.cpp',
                'src/tbb/task.cpp',
                'src/tbb/task_group_context.cpp',
                'src/tbb/tbb_main.cpp',
                'src/tbb/tbb_misc.cpp',
                'src/tbb/tbb_misc_ex.cpp',
                'src/tbb/tbb_statistics.cpp',
                'src/tbb/tbb_thread.cpp',
                'src/tbb/x86_rtm_rw_mutex.cpp',
                'src/old/concurrent_queue_v2.cpp',
                'src/old/concurrent_vector_v2.cpp',
                'src/old/spin_rw_mutex_v2.cpp',
                'src/old/task_v2.cpp',
                'src/rml/client/rml_tbb.cpp',
            ],

            'conditions': [
                ['OS=="linux"', {
                    'defines': [
                        'LINUX_DEFINE',
                    ],
                    'include_dirs': [
                        'include/linux',
                    ],
                }],

                ['OS=="win" and target_arch=="x64"', {
                    'sources': [
                        'src/tbb/intel64-masm/atomic_support.asm',
                        'src/tbb/intel64-masm/itsx.asm',
                        'src/tbb/intel64-masm/intel64_misc.asm',
                    ]
                }],

                ['OS=="win" and target_arch=="ia32"', {
                    'sources': [
                        'src/tbb/ia32-masm/atomic_support.asm',
                        'src/tbb/ia32-masm/itsx.asm',
                        'src/tbb/ia32-masm/lock_byte.asm',
                    ]
                }],

                ['OS=="win"', {
                    'defines': [
                        'WIN32',
                        '_CRT_SECURE_NO_DEPRECATE',
                        '_WIN32_WINNT=0x0600',
                        'USE_WINTHREAD',
                        '_WINDOWS',
                    ],

                    'msvs_disabled_warnings': [
                        4267,
                        4800,
                        4146,
                        4244,
                        4577,
                        4018,
                    ],


                    'msvs_settings': {
                        'VCCLCompilerTool': {
                            'AdditionalOptions': [
                                '/volatile:iso',
                            ],

                            'InlineFunctionExpansion': '1',
                            'TreatWChar_tAsBuiltInType': 'true',
                            'RuntimeTypeInfo': 'true',
                            'ForceConformanceInForLoopScope': 'true',
                        },

                    },
                }],

            ],


            'configurations': {
                'Debug': {
                    'defines': [
                        'DEBUG',
                        '_DEBUG',
                    ],

                    'cflags': ['-g', '-O0'],
                    'conditions': [
                        ['target_arch=="x64"', {
                            'msvs_configuration_platform': 'x64',
                        }],
                    ],
                    'msvs_settings': {
                        'VCCLCompilerTool': {
                            'Optimization': 0,  # /Od, no optimization
                            'MinimalRebuild': 'false',
                            'OmitFramePointers': 'false',
                            'BasicRuntimeChecks': 3,  # /RTC1
                            'RuntimeLibrary': '3',
                        },
                        'VCLinkerTool': {
                            'LinkIncremental': 2,  # enable incremental linking
                        },
                    },
                    'xcode_settings': {
                        'GCC_OPTIMIZATION_LEVEL': '0',  # stop gyp from defaulting to -Os
                    },
                },

                'Release': {
                    'defines': [
                        'NDEBUG',
                    ],

                    'msvs_settings':{
                        'VCCLCompilerTool': {
                            'WholeProgramOptimization': 'false',
                            'Optimization': '1',
                            'RuntimeLibrary': '2',
                        },
                        'VCLinkerTool': {
                            'LinkTimeCodeGeneration': '2',
                        }
                    }
                },

            },


        },
    ],
}
