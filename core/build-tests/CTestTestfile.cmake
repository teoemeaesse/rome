# CMake generated Testfile for 
# Source directory: /Users/tomasmarques/iodine/core
# Build directory: /Users/tomasmarques/iodine/core/build-tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(CoreTests "/Users/tomasmarques/iodine/core/build-tests/core_tests")
set_tests_properties(CoreTests PROPERTIES  _BACKTRACE_TRIPLES "/Users/tomasmarques/iodine/core/CMakeLists.txt;159;add_test;/Users/tomasmarques/iodine/core/CMakeLists.txt;0;")
subdirs("_deps/googletest-build")
