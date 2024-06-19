
set(LIB_NAME ultron-tcp-adapters)

add_library(${LIB_NAME} STATIC)
add_library(Ultron::Tcp::Adapters ALIAS ${LIB_NAME})

target_link_libraries(${LIB_NAME} PRIVATE Ultron::Tcp Ultron::Util)
target_sources(${LIB_NAME} PRIVATE src/vnigma-adapter.cpp)
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
