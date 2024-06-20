enable_testing()

init_target(ultron_core_tests)
to_executable()

with_sources(PUBLIC origin_tests.cpp)

link_gtest()
target_link_libraries(${TARGET_NAME} PRIVATE Ultron::Core)


add_test(AllTestsInMain ${TARGET_NAME})
