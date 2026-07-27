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
find_package(pugixml)


set(HEADERS
  src/rc_helpers.h
  src/rc_ast.h
  src/rc_constants.h
  src/rc_generator.h
  src/rc_parser.h
  src/rc_resolver.h
  src/rc_tokenizer.h
)

set(SOURCES
  src/rc_helpers.cpp
  src/rc_constants.cpp
  src/rc_generator.cpp
  src/rc_parser.cpp
  src/rc_resolver.cpp
  src/rc_tokenizer.cpp
  src/main.cpp
)

add_executable(${TARGET_NAME} ${HEADERS} ${SOURCES})

target_include_directories(${TARGET_NAME} PRIVATE src)
target_link_libraries(${TARGET_NAME} PRIVATE Qt5::Widgets pugixml::pugixml)
