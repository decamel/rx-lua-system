add_library(ultronutil INTERFACE)
add_library(Ultron::Util ALIAS ultronutil)

target_link_libraries(ultronutil INTERFACE spdlog::spdlog)
target_include_directories(ultronutil INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
