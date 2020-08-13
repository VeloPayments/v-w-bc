project('vwbc', 'c', 'cpp',
    version : '0.0.1',
    default_options : ['c_std=gnu11', 'cpp_std=c++17', 'buildtype=release'],
    meson_version : '>=0.53.0'
)

add_project_arguments('-Wall', '-Werror', '-Wextra', language : 'c')
add_project_arguments('-Wall', '-Werror', '-Wextra', language : 'cpp')

src = run_command(
    'find', './src', '-name', '*.cpp',
    check : true
).stdout().strip().split('\n')

vcblockchain = dependency(
    'vcblockchain',
    required : true,
    fallback : ['vcblockchain', 'vcblockchain_dep']
)

vwbc_include = include_directories('include')

vwbc_exe = executable(
    'vwbc',
    src,
    include_directories : vwbc_include,
    dependencies : [vcblockchain]
)
