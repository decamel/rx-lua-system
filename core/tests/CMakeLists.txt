enable_testing()

init_target(ultron_core_tests)
to_executable()

with_sources(origin_tests.cpp test_dynamic_stream.cpp PUBLIC)

link_gtest()
target_link_libraries(${TARGET_NAME} PRIVATE Ultron::Core)


add_test(AllTestsInMain ${TARGET_NAME})
