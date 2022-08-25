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
# CMake files for the srcML client

# strip executables
set(CPACK_STRIP_FILES ON)
 
set(CMAKE_EXTERNAL_SOURCE_DIR ${CMAKE_BINARY_DIR}/external)

set(CLI11_EXTERNAL ${CMAKE_EXTERNAL_SOURCE_DIR}/CLI11.hpp)
if(NOT EXISTS ${CLI11_EXTERNAL})
    file(DOWNLOAD https://github.com/CLIUtils/CLI11/releases/download/v1.8.0/CLI11.hpp ${CLI11_EXTERNAL})
endif()
set(CTPL_EXTERNAL ${CMAKE_EXTERNAL_SOURCE_DIR}/ctpl_stl.h)
if(NOT EXISTS ${CTPL_EXTERNAL})
    file(DOWNLOAD https://raw.githubusercontent.com/vit-vit/CTPL/ctpl_v.0.0.2/ctpl_stl.h ${CTPL_EXTERNAL})
endif()

set(SRCML_LIBRARIES CACHE INTERNAL "Libraries needed to build srcml")

# if(APPLE)
#     # list(APPEND SRCML_LIBRARIES pthread)
# elseif(NOT WIN32)
#     # list(APPEND SRCML_LIBRARIES pthread)
#     # list(APPEND SRCML_LIBRARIES crypto pthread)
# # elseif(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
# #     list(APPEND SRCML_LIBRARIES ws2_32 crypto)
# else()
#     # list(APPEND SRCML_LIBRARIES ws2_32)
# endif()

# srcml executable
file(GLOB CLIENT_SOURCE *.hpp *.cpp)
add_executable(srcml ${CLIENT_SOURCE})

# Add include files
if(MSVC)
    target_compile_options(srcml PRIVATE ${CMAKE_INCLUDE_SYSTEM_FLAG_CXX}${CMAKE_EXTERNAL_SOURCE_DIR})
else()
    target_include_directories(srcml SYSTEM PRIVATE ${CMAKE_EXTERNAL_SOURCE_DIR})
endif()
target_include_directories(srcml BEFORE PRIVATE . ${CMAKE_SOURCE_DIR}/src/libsrcml)

# Largest impace for PCH builds of the client
if(SRCML_BUILD_USEPCH)
    target_precompile_headers(srcml PRIVATE <srcml_cli.hpp> <srcml_input_src.hpp> <srcml_pipe.hpp> <ParseQueue.hpp> <src_input_libarchive.hpp> <SRCMLStatus.hpp> <WriteQueue.hpp> <ParseRequest.hpp> <TraceLog.hpp> <curl/curl.h>)
endif()

# Add coverage to default part of Debug
target_link_libraries(srcml libsrcml_link LibArchive::LibArchive CURL::libcurl ${SRCML_LIBRARIES})
if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND CMAKE_COMPILER_IS_GNUCXX)
    target_link_libraries(srcml gcov)
endif()

if(VCPKG)
    target_link_libraries(srcml BZip2::BZip2 LibLZMA::LibLZMA ZLIB::ZLIB)
endif()

set_target_properties(srcml PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
if(APPLE)
    # Making the exported_symbols_list an empty file reduces size of executable, as strip does not work
    set_target_properties(srcml PROPERTIES LINK_FLAGS "-exported_symbols_list /dev/null")
elseif(WIN32)
    # @BUILD Why this when there is a MSVC case above?
    set_target_properties(srcml PROPERTIES LINK_FLAGS "-Wl,--allow-multiple-definition")
endif()

# strip macOS for release
if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug" AND APPLE)
    add_custom_command(TARGET srcml POST_BUILD
        COMMAND /usr/bin/strip -x $<TARGET_FILE:srcml>
        COMMAND ${CMAKE_COMMAND} -E echo "-- Stripping $<TARGET_FILE:srcml>")
endif()

# Dependencies are installed via libsrcml
# NOTE: Need to separate what is installed here and what is installed in libsrcml
install(TARGETS srcml COMPONENT SRCML)
