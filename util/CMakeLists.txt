set(LIB_NAME ultron-util)
add_library(${LIB_NAME} INTERFACE)
add_library(Ultron::Util ALIAS ${LIB_NAME})

target_link_libraries(${LIB_NAME} INTERFACE spdlog::spdlog)

target_include_directories(${LIB_NAME}
                           INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
