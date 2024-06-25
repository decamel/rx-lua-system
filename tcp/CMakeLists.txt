
add_library(ultrontcp STATIC)
add_library(Ultron::Tcp ALIAS ultrontcp)

target_sources(ultrontcp
  PRIVATE
    ./src/session.cpp
    ./src/socket-protocol-resolution.cpp
)

target_link_libraries(ultrontcp PRIVATE Ultron::Util)

target_include_directories(ultrontcp PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
