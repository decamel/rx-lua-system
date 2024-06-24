set(LIB_NAME ultron-actions-log)

add_library(${LIB_NAME} INTERFACE)
add_library(Ultron::Actions::Log ALIAS ${LIB_NAME})

target_link_libraries(${LIB_NAME} INTERFACE Ultron::Util)

target_include_directories(${LIB_NAME}
                           INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

if(BUILD_TESTS)
  add_subdirectory(tests)
endif()
