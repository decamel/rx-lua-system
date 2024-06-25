enable_testing()

init_target(ultron_adapters_tests)
to_executable()

with_sources(PUBLIC adapter_message_glue_test.cpp)

link_gtest()
target_link_libraries(${TARGET_NAME} PRIVATE Ultron::Adapters)

add_test(AllTestsInMain ${TARGET_NAME})
