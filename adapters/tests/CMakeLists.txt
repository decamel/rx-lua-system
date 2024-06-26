enable_testing()

init_target(ultron_adapters_tests)
to_executable()

with_sources(PUBLIC configuration_repository_tests.cpp)

link_gtest()
target_link_libraries(${TARGET_NAME} PRIVATE Ultron::Adapters vnigma::vnigma)

add_test(AllTestsInMain ${TARGET_NAME})
