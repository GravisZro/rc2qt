cmake_minimum_required(VERSION 3.16)

option(BUILD_WITH_QT "Build the project using Qt (slower execution)" OFF)

set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON" FORCE)

set(TARGET_NAME "rc2qt")

project(${TARGET_NAME}
    DESCRIPTION ${TARGET_NAME}
    LANGUAGES CXX
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# check for missing headers
include(CheckIncludeFileCXX)
check_include_file_cxx(format HAVE_STDFORMAT_HEADER)

if(HAVE_STDFORMAT_HEADER)
  set(EXTRA_DIRS "")
  set(EXTRA_LIBS "")
else()
  find_package(fmt REQUIRED)
  set(EXTRA_DIRS polyfill)
  set(EXTRA_LIBS fmt::fmt)
endif()

if(BUILD_WITH_QT)
  set(CMAKE_AUTOMOC ON)
  target_compile_definitions(${TARGET_NAME} PRIVATE HAVE_QT)
  find_package(Qt5 REQUIRED COMPONENTS Widgets)
  list(APPEND EXTRA_LIBS Qt5::Widgets)
  # temporary until Qt transition
  find_package(pugixml REQUIRED)
  list(APPEND EXTRA_LIBS pugixml::pugixml)
else()
#xml stuff
  find_package(pugixml REQUIRED)
  list(APPEND EXTRA_LIBS pugixml::pugixml)
# font stuff
  find_package(Freetype REQUIRED)
  find_package(Fontconfig REQUIRED)
  list(APPEND EXTRA_LIBS Freetype::Freetype)
  list(APPEND EXTRA_LIBS Fontconfig::Fontconfig)
  list(APPEND EXTRA_DIRS ${FREETYPE_INCLUDE_DIRS})
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0")


set(BUILD_SHARED_LIBS OFF)
add_subdirectory(libpefile)
set(BUILD_SHARED_LIBS ON)


set(HEADERS
  src/utils.h
  src/xmlhelpers.h
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
  src/utils.cpp
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
target_link_libraries(${TARGET_NAME} PRIVATE pefile ${EXTRA_LIBS})

# Parser unit tests
set(TEST_SOURCES
  src/rc_tokenizer.cpp
  src/rc_constants.cpp
  src/rc_resolver.cpp
  src/utils.cpp
  src/rc_parser.cpp
  src/test_parser.cpp
)

add_executable(test_parser ${TEST_SOURCES})
target_include_directories(test_parser PRIVATE src ${EXTRA_DIRS})
target_link_libraries(test_parser PRIVATE ${EXTRA_LIBS})

enable_testing()
add_test(NAME parser_tests COMMAND test_parser)
