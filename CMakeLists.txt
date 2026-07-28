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

# check for missing headers
include(CheckIncludeFileCXX)
check_include_file_cxx(format HAVE_STDFORMAT_HEADER)

if(HAVE_STDFORMAT_HEADER)
  set(EXTRA_DIRS "")
  set(EXTRA_LIBS "")
else()
  find_package(fmt)
  set(EXTRA_DIRS polyfill)
  set(EXTRA_LIBS fmt::fmt)
endif()

find_package(Qt5 REQUIRED COMPONENTS Widgets)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0")
find_package(pugixml)

set(BUILD_SHARED_LIBS OFF)
add_subdirectory(libpefile)
set(BUILD_SHARED_LIBS ON)


set(HEADERS
  src/rc_helpers.h
  src/rc_ast.h
  src/rc_constants.h
  src/rc_generator.h
  src/rc_parser.h
  src/rc_resolver.h
  src/rc_tokenizer.h
  src/pe_resource_decoder.h
  src/imageio.h
)

set(SOURCES
  src/rc_helpers.cpp
  src/rc_constants.cpp
  src/rc_generator.cpp
  src/rc_parser.cpp
  src/rc_resolver.cpp
  src/rc_tokenizer.cpp
  src/pe_resource_decoder.cpp
  src/imageio.cpp
  src/main.cpp
)

add_executable(${TARGET_NAME} ${HEADERS} ${SOURCES})

target_include_directories(${TARGET_NAME} PRIVATE src ${EXTRA_DIRS})
target_link_libraries(${TARGET_NAME} PRIVATE Qt5::Widgets pugixml::pugixml pefile ${EXTRA_LIBS})

# Parser unit tests
set(TEST_SOURCES
  src/rc_tokenizer.cpp
  src/rc_constants.cpp
  src/rc_resolver.cpp
  src/rc_helpers.cpp
  src/rc_parser.cpp
  src/test_parser.cpp
)

add_executable(test_parser ${TEST_SOURCES})
target_include_directories(test_parser PRIVATE src ${EXTRA_DIRS})
target_link_libraries(test_parser PRIVATE pugixml::pugixml ${EXTRA_LIBS})

enable_testing()
add_test(NAME parser_tests COMMAND test_parser)

add_executable(pe_dump tests/pe_dump.cpp)
target_link_libraries(pe_dump PRIVATE pefile)

add_executable(menu_debug tests/menu_debug.cpp)
target_link_libraries(menu_debug PRIVATE pefile)

add_executable(menuex_debug tests/menuex_debug.cpp)
target_link_libraries(menuex_debug PRIVATE pefile)
