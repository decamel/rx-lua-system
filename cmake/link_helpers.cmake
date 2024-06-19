
macro (init_target NAME)
    set (TARGET_NAME ${NAME})
endmacro ()

macro (to_executable)
  add_executable(${TARGET_NAME})
endmacro ()

macro(with_sources SOURCES)
  target_sources(${TARGET_NAME} ${ARGN} ${SOURCES})
endmacro()

macro (link_gtest) 
  target_link_libraries(${TARGET_NAME} PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
endmacro()

macro (link_core) 
  target_link_libraries(${TARGET_NAME} ${ARGN} Ultron::Core)
endmacro()

