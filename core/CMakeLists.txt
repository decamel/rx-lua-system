
set(LIB_NAME ultron-core)

add_library(${LIB_NAME} INTERFACE)
add_library(Ultron::Core ALIAS ${LIB_NAME})

target_include_directories(${LIB_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(${LIB_NAME} INTERFACE nlohmann_json::nlohmann_json)


if(BUILD_TESTS)
add_subdirectory(tests)
endif()
