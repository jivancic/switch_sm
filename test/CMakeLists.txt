cmake_minimum_required (VERSION 2.8)
project (SwitchSMTests)

set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        )
foreach(CompilerFlag ${CompilerFlags})
  string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
endforeach()

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
set(gtest_force_shared_crt ON CACHE BOOL "Always use msvcrt.dll")

add_executable(test_simple test_simple.cpp)
target_link_libraries(test_simple ${GTEST_BOTH_LIBRARIES})

add_test(TestSimple test_simple)

add_custom_command(TARGET test_simple POST_BUILD COMMAND test_simple)