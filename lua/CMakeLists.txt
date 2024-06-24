set(LIB_NAME ultron-lua-engine)
add_library(${LIB_NAME} STATIC)

add_library(Ultron::Lua::Engine ALIAS ${LIB_NAME})

target_sources(${LIB_NAME} PRIVATE ./src/operator.cpp)

target_link_libraries(${LIB_NAME} PRIVATE Ultron::Util)

target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
