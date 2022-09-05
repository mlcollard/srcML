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
# CMake files for libsrcml tests

# Build and add tests
add_custom_target(build_libsrcml_tests)
file(GLOB LIB_TESTS test_*.cpp)
foreach(LIB_TEST ${LIB_TESTS})

    # Test name is filename without extension
    get_filename_component(TEST_NAME ${LIB_TEST} NAME_WE)
    add_executable(${TEST_NAME} ${LIB_TEST})
    set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

    # Link to libsrcml
    target_link_libraries(${TEST_NAME} ${LIBSRCML_LINK})
    if (STATIC)
        target_link_libraries(${TEST_NAME} LibXml2::LibXml2)
    endif()

    # Create a test with its own test directory
    set(TEST_DIR ${CMAKE_BINARY_DIR}/bin/tmp/${TEST_NAME})
    file(MAKE_DIRECTORY ${TEST_DIR})
    add_test(NAME ${TEST_NAME} COMMAND $<TARGET_FILE:${TEST_NAME}> WORKING_DIRECTORY ${TEST_DIR})
    set_tests_properties(${TEST_NAME} PROPERTIES TIMEOUT 15)

    # Copy over all files needed by any of the tests
    configure_file(copy.xsl ${TEST_DIR}/copy.xsl COPYONLY)
    configure_file(setlanguage.xsl ${TEST_DIR}/setlanguage.xsl COPYONLY)
    configure_file(schema.rng ${TEST_DIR}/schema.rng COPYONLY)

    # Add this test to all the libsrcml tests
    add_dependencies(build_libsrcml_tests ${TEST_NAME})
endforeach()

# Copy over all files needed by the tests into the bin directory so they can easily be run outside
# of ctest
configure_file(copy.xsl ${CMAKE_BINARY_DIR}/bin/copy.xsl COPYONLY)
configure_file(setlanguage.xsl ${CMAKE_BINARY_DIR}/bin/setlanguage.xsl COPYONLY)
configure_file(schema.rng ${CMAKE_BINARY_DIR}/bin/schema.rng COPYONLY)
