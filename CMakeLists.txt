cmake_minimum_required(VERSION 3.10.0)
project(tftp)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/src )
get_property(dirs DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
message(">>> include_dirs=${dirs}")

aux_source_directory(src/msg MSG_SRCS)

set(MSG_TEST_SRCS 
    test/MsgTest.cpp 
    ${MSG_SRCS})

add_executable(MsgTest ${MSG_TEST_SRCS})
