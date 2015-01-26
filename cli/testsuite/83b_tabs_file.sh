#!/bin/bash

# test framework
source $(dirname "$0")/framework_test.sh

# test tabs

define fsrcml <<- 'STDOUT'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" xmlns:pos="http://www.sdml.info/srcML/position" revision="0.8.0" language="C++" filename="sub/a.cpp" pos:tabs="4"/>
	STDOUT

define srcml <<- 'STDOUT'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" xmlns:pos="http://www.sdml.info/srcML/position" revision="0.8.0" language="C++" pos:tabs="4"/>
	STDOUT

createfile sub/a.cpp ""

srcml -l C++ --tabs 4 < sub/a.cpp
check 3<<< "$srcml"

srcml --tabs 4 sub/a.cpp
check 3<<< "$fsrcml"

srcml -l C++ --tabs 4 -o sub/a.cpp.xml < sub/a.cpp
check sub/a.cpp.xml 3<<< "$srcml"

srcml --tabs 4 sub/a.cpp -o sub/a.cpp.xml
check sub/a.cpp.xml 3<<< "$fsrcml"

srcml --tabs 4 -o sub/a.cpp.xml sub/a.cpp
check sub/a.cpp.xml 3<<< "$fsrcml"