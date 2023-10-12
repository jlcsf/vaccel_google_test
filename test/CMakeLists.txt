include(GoogleTest)

set(include_dirs ${CMAKE_SOURCE_DIR}/src)

file(GLOB tests "${PROJECT_SOURCE_DIR}/test/*.cpp")
list(REMOVE_ITEM tests "${PROJECT_SOURCE_DIR}/test/tests.cpp")

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
target_compile_options(plugin_tests PUBLIC -Wall -Wextra -g -ggdb)
target_link_libraries("plugin_tests" gtest_main dl slog)
gtest_add_tests(TARGET plugin_tests)


# resource system unit test

set(resources_src "${PROJECT_SOURCE_DIR}/src/resources.c")
add_executable(
	resources_tests
	test_resources.cpp ${resources_src}
)
target_include_directories(
	resources_tests
	PRIVATE
	${GTEST_INCLUDE} ${include_dirs}
)
target_compile_options(resources_tests PUBLIC -Wall -Wextra -g -ggdb)
target_link_libraries("resources_tests" gtest gtest_main dl slog)
gtest_add_tests(TARGET resources_tests LANGUAGE C)
