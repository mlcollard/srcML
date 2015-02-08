#!/bin/bash

# test framework
source $(dirname "$0")/framework_test.sh

# test

##
# version flag
define srcml <<- 'STDOUT'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" revision="REVISION" language="C++" version="1.0"/>
	STDOUT

# file input
define fsrcml <<- 'STDOUT'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" revision="REVISION" language="C++" filename="sub/a.cpp" version="1.0"/>
	STDOUT

createfile sub/a.cpp ""

src2srcml sub/a.cpp -s "1.0"

check 3<<< "$fsrcml"

src2srcml sub/a.cpp --src-version "1.0"

check 3<<< "$fsrcml"

src2srcml sub/a.cpp --src-version="1.0"

check 3<<< "$fsrcml"

src2srcml -l C++ -s '1.0' -o sub/a.cpp.xml sub/a.cpp

check sub/a.cpp.xml 3<<< "$fsrcml"

src2srcml -s '1.0' sub/a.cpp -o sub/a.cpp.xml

check sub/a.cpp.xml 3<<< "$fsrcml"

# standard input
src2srcml -l C++ -s '1.0' -o sub/a.cpp.xml < sub/a.cpp

check sub/a.cpp.xml 3<<< "$srcml"