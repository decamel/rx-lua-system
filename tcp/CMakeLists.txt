

add_library(ultrontcp STATIC)
add_library(Ultron::Tcp ALIAS ultrontcp)

target_link_libraries(ultrontcp PRIVATE Ultron::Util)
target_sources(ultrontcp PRIVATE src/server.cpp src/session.cpp)
target_include_directories(ultrontcp PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
