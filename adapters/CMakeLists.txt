set(LIB_NAME ultron-adapters)

add_library(${LIB_NAME} INTERFACE)
add_library(Ultron::Adapters ALIAS ${LIB_NAME})

target_link_libraries(${LIB_NAME} INTERFACE Boost::boost spdlog::spdlog Ultron::Util)
target_include_directories(${LIB_NAME}
                           INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
