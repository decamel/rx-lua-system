add_library(ultrontcp INTERFACE)
add_library(Ultron::Tcp ALIAS ultrontcp)

# target_sources(ultrontcp PRIVATE)

target_link_libraries(ultrontcp INTERFACE Ultron::Util)

target_include_directories(ultrontcp
                           INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
