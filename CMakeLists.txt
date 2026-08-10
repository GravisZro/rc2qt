cmake_minimum_required(VERSION 3.16)

option(BUILD_WITH_QT  "Build the project using Qt (slower execution)" OFF)
option(BUILD_WITH_ICU "Build the project with unlimited code page support" ON)


set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON" FORCE)

set(TARGET_NAME "rc2qt")

project(${TARGET_NAME}
    DESCRIPTION ${TARGET_NAME}
    LANGUAGES CXX
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(EXTRA_DIRS "")
set(EXTRA_LIBS "")
set(EXTRA_DEFS "")

# check for missing format header (G++ 12)
include(CheckIncludeFileCXX)
check_include_file_cxx(format HAVE_STDFORMAT_HEADER)

if(NOT HAVE_STDFORMAT_HEADER)
  find_package(fmt REQUIRED)
  list(APPEND EXTRA_DIRS polyfill)
  list(APPEND EXTRA_LIBS fmt::fmt)
  list(APPEND EXTRA_DEFS HAVE_FORMAT)
endif()

if(BUILD_WITH_QT)
  set(CMAKE_AUTOMOC ON)
  find_package(Qt5 REQUIRED COMPONENTS Widgets)
  list(APPEND EXTRA_LIBS Qt5::Widgets)
  list(APPEND EXTRA_DEFS HAVE_QT)

  # temporary until Qt transition
  find_package(pugixml REQUIRED)
  list(APPEND EXTRA_LIBS pugixml::pugixml)
  list(APPEND EXTRA_DEFS HAVE_PUGIXML)
else()
#xml stuff
  find_package(pugixml REQUIRED)
  list(APPEND EXTRA_LIBS pugixml::pugixml)
  list(APPEND EXTRA_DEFS HAVE_PUGIXML)

# font stuff
  find_package(Freetype REQUIRED)
  find_package(Fontconfig REQUIRED)
  list(APPEND EXTRA_LIBS Freetype::Freetype)
  list(APPEND EXTRA_LIBS Fontconfig::Fontconfig)
  list(APPEND EXTRA_DIRS ${FREETYPE_INCLUDE_DIRS})
  list(APPEND EXTRA_DEFS HAVE_FREETYPE)
endif()

if(BUILD_WITH_ICU)
  find_package(ICU REQUIRED COMPONENTS uc i18n)
  list(APPEND EXTRA_LIBS ICU::uc ICU::i18n)
  list(APPEND EXTRA_DEFS HAVE_ICU)
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
  src/rc_layout.h
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
  src/rc_layout.cpp
  src/main.cpp
)

if(BUILD_WITH_QT)
  list(APPEND SOURCES src/rc_render.cpp)
endif()

add_executable(${TARGET_NAME} ${HEADERS} ${SOURCES})

target_include_directories(${TARGET_NAME} PRIVATE src ${EXTRA_DIRS})
target_link_libraries(${TARGET_NAME} PRIVATE pefile ${EXTRA_LIBS})
target_compile_definitions(${TARGET_NAME} PRIVATE ${EXTRA_DEFS})

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
target_compile_definitions(test_parser PRIVATE ${EXTRA_DEFS})

enable_testing()
add_test(NAME parser_tests COMMAND test_parser)


# rc_overlap: reports overlapping controls in a parsed .rc dialog layout.
# Overlaps are detected in source DLU coordinates so they can be classified
# as source-inherited (by design) versus introduced by the converter.
add_executable(rc_overlap
  src/rc_tokenizer.cpp
  src/rc_constants.cpp
  src/rc_resolver.cpp
  src/utils.cpp
  src/rc_parser.cpp
  tools/rc_overlap.cpp
)
target_include_directories(rc_overlap PRIVATE src ${EXTRA_DIRS})
target_link_libraries(rc_overlap PRIVATE ${EXTRA_LIBS})
target_compile_definitions(rc_overlap PRIVATE ${EXTRA_DEFS})

# ui_overlap: reports overlapping widgets in a generated .ui file.
# Siblings are compared within their own coordinate space (dialog, groupbox,
# tab page); tab pages stacked by design are excluded from sibling checks.
add_executable(ui_overlap
  tools/ui_overlap.cpp
)
target_include_directories(ui_overlap PRIVATE src)
target_link_libraries(ui_overlap PRIVATE pugixml::pugixml)


# getuimetrics: standalone Qt tool that renders rc2qt widget classes
# offscreen and writes their component metrics to a text file for the
# generator.
option(BUILD_GETUIMETRICS "Build the getuimetrics Qt widget metrics tool" ON)
if(BUILD_GETUIMETRICS)
  find_package(Qt5 REQUIRED COMPONENTS Widgets)
  add_executable(getuimetrics
    src/getuimetrics.cpp
  )
  target_link_libraries(getuimetrics PRIVATE Qt5::Widgets)
endif()
