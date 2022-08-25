##
# @file CMakeLists.txt
# 
# @copyright Copyright (C) 2013-2019 srcML, LLC. (www.srcML.org)
# 
# The srcML Toolkit is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# The srcML Toolkit is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with the srcML Toolkit; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
# 
# CMake files for the library libsrcml

# strip executables
# Note: Does not seem to have an effect, so post-build strip is added
set(CPACK_STRIP_FILES ON)

if(DYNAMIC_LOAD_ENABLED)
    set(LIBSRCML_LIBRARIES dl pthread
                CACHE INTERNAL "Libraries needed to build libsrcml")
elseif(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(LIBSRCML_LIBRARIES pthread
                CACHE INTERNAL "Libraries needed to build libsrcml")
else()
    set(LIBSRCML_LIBRARIES
                CACHE INTERNAL "Libraries needed to build libsrcml")
endif()

if(NOT WIN32 AND NOT APPLE)
    list(APPEND LIBSRCML_LIBRARIES rt crypto)
endif()

include_directories(BEFORE . ${CMAKE_SOURCE_DIR}/src/parser ${CMAKE_INSTALL_INCLUDEDIR})

# Setup SHA1 include
set(CMAKE_EXTERNAL_SOURCE_DIR ${CMAKE_BINARY_DIR}/external)
set(SHA1_EXTERNAL ${CMAKE_EXTERNAL_SOURCE_DIR}/TinySHA1.hpp)
if(NOT EXISTS ${SHA1_EXTERNAL})
    file(DOWNLOAD https://raw.githubusercontent.com/mohaps/TinySHA1/master/TinySHA1.hpp ${SHA1_EXTERNAL})
endif()

# Building static and dynamic libraries for srcML.
add_library(libsrcml_static STATIC $<TARGET_OBJECTS:parser> $<TARGET_OBJECTS:antlr> $<TARGET_OBJECTS:libsrcml>)
set_target_properties(libsrcml_static PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
target_include_directories(libsrcml_static BEFORE PRIVATE . ${CMAKE_SOURCE_DIR}/src/parser)
target_link_libraries(libsrcml_static PRIVATE LibXml2::LibXml2 Iconv::Iconv LibXslt::LibXslt LibXslt::LibExslt ${LIBSRCML_LIBRARIES})
if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND CMAKE_COMPILER_IS_GNUCXX)
    target_link_libraries(libsrcml_static PRIVATE gcov)
endif()

add_library(libsrcml_shared SHARED $<TARGET_OBJECTS:parser> $<TARGET_OBJECTS:antlr> $<TARGET_OBJECTS:libsrcml>)
set_target_properties(libsrcml_shared PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
if(WIN32)
    target_compile_options(libsrcml_shared PRIVATE ${CMAKE_INCLUDE_SYSTEM_FLAG_CXX}${LIBXML2_INCLUDE_DIRS})
    target_compile_options(libsrcml_shared PRIVATE ${CMAKE_INCLUDE_SYSTEM_FLAG_CXX}${LIBXML2_INCLUDE_DIR})
    target_compile_options(libsrcml_shared PRIVATE ${CMAKE_INCLUDE_SYSTEM_FLAG_CXX}${Boost_INCLUDE_DIR})
    target_compile_options(parser PRIVATE ${CMAKE_INCLUDE_SYSTEM_FLAG_CXX}${ANTLR_INCLUDE_DIR})
endif()
target_link_libraries(libsrcml_shared PUBLIC LibXml2::LibXml2 Iconv::Iconv LibXslt::LibXslt LibXslt::LibExslt ${LIBSRCML_LIBRARIES})
if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND CMAKE_COMPILER_IS_GNUCXX)
    target_link_libraries(libsrcml_shared PRIVATE gcov)
endif()

set_target_properties(libsrcml_shared PROPERTIES VERSION "${PROJECT_VERSION}" SOVERSION "${PROJECT_VERSION_MAJOR}")

if(MSVC)
    set_target_properties(libsrcml_shared PROPERTIES OUTPUT_NAME libsrcml LINK_FLAGS_DEBUG "/SAFESEH:NO" LINK_FLAGS_RELEASE "/DEBUG /OPT:REF /OPT:ICF")
    set_target_properties(libsrcml_static PROPERTIES OUTPUT_NAME libsrcmls LINK_FLAGS_RELEASE "/LTCG:OFF")
elseif(APPLE)
    set_target_properties(libsrcml_shared libsrcml_static PROPERTIES OUTPUT_NAME srcml LINK_FLAGS "-Wl,-exported_symbols_list ${CMAKE_SOURCE_DIR}/src/libsrcml/export_list" MACOSX_RPATH OFF)
else()
    set_target_properties(libsrcml_shared libsrcml_static PROPERTIES OUTPUT_NAME srcml)
endif()

# which types of libraries
if(MSVC)

    # libsrcml shared
    install(TARGETS libsrcml_shared COMPONENT SRCML)

    # pdb file for the MSVC shared version
    install(FILES $<TARGET_PDB_FILE:libsrcml_shared> DESTINATION bin COMPONENT DEVLIBS)
else()
    install(TARGETS libsrcml_shared LIBRARY COMPONENT SRCML NAMELINK_COMPONENT DEVLIBS)
endif()

if (BUILD_LIBSRCML_STATIC)
    if(MSVC)
        # do not use static library for MSVC
        install(TARGETS libsrcml_static COMPONENT DEVLIBS)
    else()
        install(TARGETS libsrcml_static COMPONENT SRCML)
    endif()
endif()

if(LINK_LIBSRCML_STATIC AND BUILD_LIBSRCML_STATIC)
    add_library(libsrcml_link ALIAS libsrcml_static)
else()
    add_library(libsrcml_link ALIAS libsrcml_shared)
endif()

if(APPLE)
#    add_custom_command(TARGET libsrcml_shared POST_BUILD COMMAND /usr/bin/install_name_tool -id /usr/local/lib/libsrcml.dylib /usr/local/lib/libsrcml.dylib)
endif()

# libsrcml library
file(GLOB LIBSRCML_SOURCE *.h *.hpp *.cpp)
add_library(libsrcml OBJECT ${LIBSRCML_SOURCE})

# generated source include files
target_include_directories(libsrcml SYSTEM PRIVATE ${CMAKE_GENERATED_SOURCE_DIR})

# libsrcml directory and parser
target_include_directories(libsrcml BEFORE PRIVATE . ${CMAKE_SOURCE_DIR}/src/parser)

# external source directories
target_include_directories(libsrcml PRIVATE ${CMAKE_EXTERNAL_SOURCE_DIR})

# precompiled header list of most commonly used
if(SRCML_BUILD_USEPCH)
    target_precompile_headers(libsrcml PRIVATE <srcmlns.hpp> <srcml_types.hpp> <srcml_sax2_utilities.hpp> <srcml_translator.hpp> <srcml_sax2_reader.hpp>)
endif()

# libsrcml depends on the parser
add_dependencies(libsrcml parser)

# install the libsrcml include file
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/srcml.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT DEVLIBS)

# on Windows with vcpkg, install all the dlls that the vcpkg installed
# NOTE: Would be better to use $<TARGET_RUNTIME_DLLS:libsrcml_shared>, but that does not include
# indirect dependencies, e.g., bzip2 for libarchive
if(MSVC AND VCPKG)
    file(GLOB VCPKG_SRCML_DEPENDENCIES "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/*${CMAKE_SHARED_LIBRARY_SUFFIX}")
    install(FILES ${VCPKG_SRCML_DEPENDENCIES} DESTINATION bin COMPONENT SRCML)
endif()

# strip libraries
if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(APPLE)
        add_custom_command(TARGET libsrcml_shared POST_BUILD
            COMMAND /usr/bin/strip -x $<TARGET_FILE:libsrcml_shared>
            COMMAND ${CMAKE_COMMAND} -E echo "-- Stripping $<TARGET_FILE:libsrcml_shared>")
    elseif(NOT WIN32)
        set_target_properties(libsrcml_shared PROPERTIES LINK_FLAGS_RELEASE -s)
    endif()
endif()

# Run ldconfig for install (not cpack) on Ubuntu
if(DISTRO AND DISTRO MATCHES "Ubuntu")
    install(CODE "message(STATUS \"Running: ldconfig\")")
    install(CODE "execute_process(COMMAND ldconfig)")
endif()
