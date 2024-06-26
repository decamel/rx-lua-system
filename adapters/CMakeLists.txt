set(LIB_NAME ultron-adapters)

add_library(${LIB_NAME} STATIC)
add_library(Ultron::Adapters ALIAS ${LIB_NAME})

target_sources(${LIB_NAME} PRIVATE ./src/das/configuration-repository.cpp
                                   ./src/das/resolution-queue.cpp)

target_link_libraries(${LIB_NAME} PUBLIC Boost::boost spdlog::spdlog
                                         Ultron::Util vnigma::vnigma)

target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

if(BUILD_TESTS)
  add_subdirectory(tests)
endif()
