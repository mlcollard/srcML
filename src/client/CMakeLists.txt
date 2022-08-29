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
 
include(FetchContent)
FetchContent_Declare(cli11
  URL https://github.com/CLIUtils/CLI11/releases/download/v1.8.0/CLI11.hpp
  DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_MakeAvailable(cli11)
add_library(cli11 INTERFACE IMPORTED)
target_include_directories(cli11 INTERFACE ${cli11_SOURCE_DIR})

# ctpl_stl external include
include(FetchContent)
FetchContent_Declare(ctpl_stl
  GIT_REPOSITORY https://github.com/vit-vit/CTPL.git
  GIT_TAG        v.0.0.2
  GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(ctpl_stl)
add_library(ctpl_stl INTERFACE IMPORTED)
target_include_directories(ctpl_stl INTERFACE ${ctpl_stl_SOURCE_DIR})

# srcml executable
file(GLOB CLIENT_SOURCE *.hpp *.cpp)
add_executable(srcml ${CLIENT_SOURCE})

# Add include files
if(MSVC)
    target_compile_options(srcml PRIVATE ${CMAKE_INCLUDE_SYSTEM_FLAG_CXX}${cli11_SOURCE_DIR})
endif()
target_include_directories(srcml BEFORE PRIVATE . ${CMAKE_SOURCE_DIR}/src/libsrcml)

# Largest impace for PCH builds of the client
if(SRCML_BUILD_USEPCH)
    target_precompile_headers(srcml PRIVATE <srcml_cli.hpp> <srcml_input_src.hpp> <srcml_pipe.hpp> <ParseQueue.hpp> <src_input_libarchive.hpp> <SRCMLStatus.hpp> <WriteQueue.hpp> <ParseRequest.hpp> <TraceLog.hpp> <curl/curl.h>)
endif()

# Add coverage to default part of Debug
target_link_libraries(srcml libsrcml_link LibArchive::LibArchive CURL::libcurl cli11 ctpl_stl)
if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND CMAKE_COMPILER_IS_GNUCXX)
    target_link_libraries(srcml gcov)
endif()

set_target_properties(srcml PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
if(APPLE)
    # Making the exported_symbols_list an empty file reduces size of executable, as strip does not work
    set_target_properties(srcml PROPERTIES LINK_FLAGS "-exported_symbols_list /dev/null")
elseif(WIN32)
    set_target_properties(srcml PROPERTIES LINK_FLAGS "-Wl,--allow-multiple-definition")
endif()

# Dependencies are installed via libsrcml
# NOTE: Need to separate what is installed here and what is installed in libsrcml
install(TARGETS srcml COMPONENT SRCML)
