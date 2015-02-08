#!/bin/bash

# test framework
source $(dirname "$0")/framework_test.sh

# test get directory
define input <<- 'STDOUT'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" revision="REVISION" language="C++" directory="bar" filename="foo" version="1.2"/>
	STDOUT

createfile sub/a.cpp.xml "$input"

srcml --show-directory sub/a.cpp.xml
check 3<<< "bar"

srcml --show-directory < sub/a.cpp.xml
check 3<<< "bar"

# empty
define input <<- 'STDOUT'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp" revision="REVISION" language="" directory="" filename="" version=""/>
	STDOUT

createfile sub/a.cpp.xml "$input"

srcml --show-directory sub/a.cpp.xml
check 3<<< ""

srcml --show-directory < sub/a.cpp.xml
check 3<<< ""

# none
define none <<- 'STDIN'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.sdml.info/srcML/src" xmlns:cpp="http://www.sdml.info/srcML/cpp"/>
	STDIN

createfile sub/a.cpp.xml "$none"

srcml --show-directory sub/a.cpp.xml
check_null

srcml --show-directory < sub/a.cpp.xml
check_null
