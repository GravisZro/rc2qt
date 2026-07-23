cmake_minimum_required(VERSION 3.16)

set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON" FORCE)

set(TARGET_NAME "rc2qt")

project(${TARGET_NAME}
    DESCRIPTION ${TARGET_NAME}
    LANGUAGES CXX
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0")

set(RESOURCES
  res/editorutf8.rc
  res/editor.rc 
  res/structures.json
)

set(HEADERS
  src/bytestream/fixed_string.h
  src/bytestream/istream_variants.h
  src/bytestream/portable_endian.h
  src/bytestream/posix_stream.h
  src/rc_ast.h
  src/rc_parser.h
  src/rc_tokenizer.h
  src/rcdef.h
  src/rctype_parsers.h
  src/shortjson.h
)

set(SOURCES
  src/bytestream/posix_stream.cpp
  src/rc_parser.cpp
  src/rc_tokenizer.cpp
  src/shortjson_strict.cpp
  src/main.cpp
  src/rctype_parsers.cpp
)

add_executable(${TARGET_NAME} ${HEADERS} ${SOURCES})

target_include_directories(${TARGET_NAME} PRIVATE src)
target_link_libraries(${TARGET_NAME} PRIVATE Qt5::Widgets)
