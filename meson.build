# Basic executable project.
#
# This project also uses the other two libraries as dependencies,
# as seen in "project_dependencies". I'm using symlinks for the subprojects,
# but ideally they should be subdirs of the subprojects dir, added via git
# submodule.
#
# All of the commonly modified parts are above the line. Generally, you
# shouldn't need to modify anything below the line until your project becomes
# fairly complex.


project(
  'arca',
  'cpp',
  version : '1.1.0',
  default_options : ['warning_level=3', 'cpp_std=c++11','cpp_link_args=-static']
)

project_source_files = [
   'src/test-tester.cpp'
]

project_dependencies = [
  #  dependency('mylib', fallback : ['mylib', 'mylib_dep']),
  # dependency('headeronly', fallback : ['headeronly', 'headeronly_dep']),
]

incdir = include_directories('include')

build_args = [
]


# ===================================================================

# ======
# Target
# ======

build_args += [
  '-DPROJECT_NAME=' + meson.project_name(),
  '-DPROJECT_VERSION=' + meson.project_version(),
]

#project_target = executable(
#  meson.project_name(),
#  project_source_files,
#  dependencies: project_dependencies,
#  install : true,
#  cpp_args : build_args,
#)


test_tester = executable('test-tester','src/test-tester.cpp', include_directories : incdir)
#test_array_fn = executable('test-array_fn','src/test-array_fn.cpp', include_directories : incdir)
#test_list_fn = executable('test-list_fn','src/test-list_fn.cpp', include_directories : incdir)
test_arca = executable('test-arca','src/test-arca.cpp', include_directories : incdir)
test_slip_cache = executable('test-slip-cache','src/test-slip-cache.cpp', include_directories : incdir)
test_slip_cache_stats = executable('test-slip-cache-stats','src/test-slip-cache-stats.cpp', include_directories : incdir)

#test_tree = executable('test-tree','src/test-tree.cpp', include_directories : incdir)
#test_space = executable('test-space','src/test-space.cpp', include_directories : incdir)

#test_arca_tree = executable('test-arca-tree','src/test-arca-tree.cpp', include_directories : incdir)
#test_tester = executable('test-chain_fn','src/test-chain_fn.cpp', include_directories : incdir)
#test('test-tester',test_tester)

