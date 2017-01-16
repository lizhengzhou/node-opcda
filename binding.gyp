{
  'targets': [
    {
      'target_name': 'opcda',
      'sources': [
        'src/opcda.cpp',
      ],
      'include_dirs': [
        'include'
      ],
      'conditions': [
        ['OS=="win"',
          {
            'msvs_settings': {
              'VCCLCompilerTool': {
                'ExceptionHandling': '2',
                'DisableSpecificWarnings': [ '4530', '4506' ],
              },
              'VCLinkerTool': {
                'AdditionalOptions':['/HIGHENTROPYVA:NO'],
                'AdditionalDependencies': [ 'OPCClientToolKit64.lib'],
                'AdditionalLibraryDirectories': ['..\\lib\\x64' ]
              },
            },
          },
        ],
      ],
    }
  ],
}
