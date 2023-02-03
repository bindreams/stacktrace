cmake_minimum_required(VERSION 3.23.0)
project(stacktrace VERSION 0.1.0)

include(cmake/warnings.cmake)
include(cmake/packaging.cmake)

#option(STACKTRACE_TESTS "Build tests for stacktrace" OFF)
option(STACKTRACE_EXAMPLES "Build examples for stacktrace" OFF)

add_library(stacktrace INTERFACE)
add_library(stacktrace::stacktrace ALIAS stacktrace)
file(GLOB_RECURSE stacktrace_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS "src/*.hpp")
target_sources(stacktrace INTERFACE FILE_SET HEADERS BASE_DIRS src FILES ${stacktrace_HEADERS})
target_include_directories(stacktrace INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
	$<INSTALL_INTERFACE:include>
)
target_link_libraries(stacktrace INTERFACE dbghelp)
target_compile_features(stacktrace INTERFACE cxx_std_17)
target_package(stacktrace)

# if(STACKTRACE_TESTS)
# 	add_executable(stacktrace_tests "tests/main.cpp")
# 	target_compile_warnings(stacktrace_tests treat_as_errors gnu_all gnu_extra ms_1)
# 	target_link_libraries(stacktrace_tests PRIVATE stacktrace)

# 	install(TARGETS stacktrace_tests)
# endif()

if(STACKTRACE_EXAMPLES)
	add_executable(stacktrace_example1 "examples/example1.cpp")
	target_link_libraries(stacktrace_example1 PRIVATE stacktrace)

	install(TARGETS stacktrace_example1)
endif()
