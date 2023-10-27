include(GoogleTest)

set(include_dirs
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/include
)

file(GLOB tests "${PROJECT_SOURCE_DIR}/test/*.cpp")
list(REMOVE_ITEM tests "${PROJECT_SOURCE_DIR}/test/tests.cpp")
#

SET(GCC_COVERAGE_COMPILE_FLAGS "-fprofile-arcs -ftest-coverage") 
SET(GCC_COVERAGE_LINK_FLAGS "-lgcov") 
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}") 
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}") 



# test_id unit test

set(id_pool_src "${PROJECT_SOURCE_DIR}/src/id_pool.c")
add_executable(
	id_pool_tests
	test_id_pool.cpp ${id_pool_src}
)
target_include_directories(
	id_pool_tests
	PRIVATE
	${GTEST_INCLUDE} ${include_dirs}
)
target_compile_options(id_pool_tests PUBLIC -Wall -Wextra -g -ggdb --coverage)
target_link_libraries("id_pool_tests" gtest gtest_main dl slog --coverage gcov)


# log unit test

set(log_src "${PROJECT_SOURCE_DIR}/src/log.c")
add_executable(
	log_tests
	test_log.cpp ${log_src}
)
target_include_directories(
	log_tests
	PRIVATE
	${GTEST_INCLUDE} ${include_dirs}
)
target_compile_options(log_tests PUBLIC -Wall -Wextra -g -ggdb --coverage)
target_link_libraries("log_tests" gtest gtest_main dl slog --coverage gcov)




# new plugins unit tests (TODO)



# session system unit test

set(session_src "${PROJECT_SOURCE_DIR}/src/session.c")
add_executable(
	session_tests
	test_session.cpp ${session_src}
)
target_include_directories(
	session_tests
	PRIVATE
	${GTEST_INCLUDE} ${include_dirs}
	../fff
)
target_compile_options(session_tests PUBLIC -Wall -Wextra -g -ggdb --coverage)
target_link_libraries("session_tests" gtest_main dl slog vaccel --coverage gcov)

# vaccel system unit test


set(vaccel_src "${PROJECT_SOURCE_DIR}/src/vaccel.c")
add_executable(
	vaccel_tests
	test_vaccel.cpp
)
target_include_directories(
	vaccel_tests
	PRIVATE
	${GTEST_INCLUDE} ${include_dirs}
	../fff
)
target_compile_options(vaccel_tests PUBLIC -Wall -Wextra -g -ggdb --coverage)
target_link_libraries("vaccel_tests" gtest gtest_main dl vaccel --coverage gcov)



# fpga unit test (NOOP plugin)

set(ENV{VACCEL_BACKENDS} "${CMAKE_BINARY_DIR}/plugins/noop/libvaccel-noop.so")
set(fpga_src "${PROJECT_SOURCE_DIR}/src/ops/fpga.c")
add_executable(
	fpga_tests
	test_fpga.cpp ${fpga_src}
)
target_include_directories(
	fpga_tests
	PRIVATE
	${GTEST_INCLUDE} ${include_dirs}
	${CMAKE_SOURCE_DIR}/src/ops
	../fff
)
target_compile_options(fpga_tests PUBLIC -Wall -Wextra -g -ggdb --coverage)
target_link_libraries("fpga_tests" gtest gtest_main dl vaccel vaccel-noop --coverage gcov) 



# misc unit test

set(misc_src "${PROJECT_SOURCE_DIR}/src/misc.c")
add_executable(
	misc_tests
	test_misc.cpp ${misc_src}
)
target_include_directories(
	misc_tests
	PRIVATE
	${GTEST_INCLUDE} ${include_dirs}
	../fff
)
target_compile_options(misc_tests PUBLIC -Wall -Wextra -g -ggdb --coverage)
target_link_libraries("misc_tests" gtest_main dl slog vaccel --coverage gcov)

# new resources unit tests (TODO)

set(resources_src "${PROJECT_SOURCE_DIR}/src/resources.c")
add_executable(
	resources_tests
	test_resources.cpp ${resources_src}
)
target_include_directories(
	resources_tests
	PRIVATE
	${GTEST_INCLUDE} ${include_dirs}
	../fff
)
target_compile_options(resources_tests PUBLIC -Wall -Wextra -g -ggdb --coverage)
target_link_libraries("resources_tests" gtest_main dl slog vaccel --coverage gcov)

# plugin system unit test
set(plugin_src "${PROJECT_SOURCE_DIR}/src/plugin.c")
add_executable(
	plugin_tests
	test_plugin.cpp ${plugin_src}
)
target_include_directories(
	plugin_tests
	PRIVATE
	${GTEST_INCLUDE} ${include_dirs}
)
target_compile_options(plugin_tests PUBLIC -Wall -Wextra -g -ggdb --coverage)
target_link_libraries("plugin_tests" gtest_main dl slog --coverage gcov)


gtest_add_tests(TARGET resources_tests)
gtest_add_tests(TARGET session_tests)
gtest_add_tests(TARGET id_pool_tests LANGUAGE C)
gtest_add_tests(TARGET log_tests LANGUAGE C)
gtest_add_tests(TARGET misc_tests)
gtest_add_tests(TARGET plugin_tests)
gtest_add_tests(TARGET fpga_tests LANGUAGE C)
gtest_add_tests(TARGET vaccel_tests LANGUAGE C)