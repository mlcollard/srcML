#!/bin/bash

# test framework
source $(dirname "$0")/framework_test.sh

define empty_srcml_archive <<- 'STDOUT'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.srcML.org/srcML/src" revision="REVISION" url="test">

	<unit xmlns:cpp="http://www.srcML.org/srcML/cpp" revision="REVISION" language="C++" filename="empty.cpp" hash="da39a3ee5e6b4b0d3255bfef95601890afd80709"/>

	</unit>
	STDOUT
createfile "list-empty-cpio-bz2.txt" "https://raw.githubusercontent.com/srcML/test-data/master/empty/empty.cpp.cpio.bz2"
src2srcml --files-from list-empty-cpio-bz2.txt --url="test"
check "$empty_srcml_archive"
rmfile list-empty-cpio-bz2.txt

define empty_srcml_archive <<- 'STDOUT'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.srcML.org/srcML/src" revision="REVISION" url="test">

	<unit xmlns:cpp="http://www.srcML.org/srcML/cpp" revision="REVISION" language="C++" filename="empty.cpp" hash="da39a3ee5e6b4b0d3255bfef95601890afd80709"/>

	</unit>
	STDOUT
createfile "list-empty-cpio-gz.txt" "https://raw.githubusercontent.com/srcML/test-data/master/empty/empty.cpp.cpio.gz"
src2srcml --files-from list-empty-cpio-gz.txt --url="test"
check "$empty_srcml_archive"
rmfile list-empty-cpio-gz.txt

define empty_srcml_archive <<- 'STDOUT'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.srcML.org/srcML/src" revision="REVISION" url="test">

	<unit xmlns:cpp="http://www.srcML.org/srcML/cpp" revision="REVISION" language="C++" filename="empty.cpp" hash="da39a3ee5e6b4b0d3255bfef95601890afd80709"/>

	</unit>
	STDOUT
createfile "list-empty-tar-bz2.txt" "https://raw.githubusercontent.com/srcML/test-data/master/empty/empty.cpp.tar.bz2"
src2srcml --files-from list-empty-tar-bz2.txt --url="test"
check "$empty_srcml_archive"
rmfile list-empty-tar-bz2.txt

define empty_srcml_archive <<- 'STDOUT'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.srcML.org/srcML/src" revision="REVISION" url="test">

	<unit xmlns:cpp="http://www.srcML.org/srcML/cpp" revision="REVISION" language="C++" filename="empty.cpp" hash="da39a3ee5e6b4b0d3255bfef95601890afd80709"/>

	</unit>
	STDOUT
createfile "list-empty-tar-gz.txt" "https://raw.githubusercontent.com/srcML/test-data/master/empty/empty.cpp.tar.gz"
src2srcml --files-from list-empty-tar-gz.txt --url="test"
check "$empty_srcml_archive"
rmfile list-empty-tar-gz.txt

define empty_srcml_archive <<- 'STDOUT'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.srcML.org/srcML/src" revision="REVISION" url="test">

	<unit xmlns:cpp="http://www.srcML.org/srcML/cpp" revision="REVISION" language="C++" filename="empty.cpp" hash="da39a3ee5e6b4b0d3255bfef95601890afd80709"/>

	</unit>
	STDOUT
createfile "list-empty-tbz2.txt" "https://raw.githubusercontent.com/srcML/test-data/master/empty/empty.cpp.tbz2"
src2srcml --files-from list-empty-tbz2.txt --url="test"
check "$empty_srcml_archive"
rmfile list-empty-tbz2.txt

define empty_srcml_archive <<- 'STDOUT'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.srcML.org/srcML/src" revision="REVISION" url="test">

	<unit xmlns:cpp="http://www.srcML.org/srcML/cpp" revision="REVISION" language="C++" filename="empty.cpp" hash="da39a3ee5e6b4b0d3255bfef95601890afd80709"/>

	</unit>
	STDOUT
createfile "list-empty-tgz.txt" "https://raw.githubusercontent.com/srcML/test-data/master/empty/empty.cpp.tgz"
src2srcml --files-from list-empty-tgz.txt --url="test"
check "$empty_srcml_archive"
rmfile list-empty-tgz.txt

define empty_srcml_archive <<- 'STDOUT'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.srcML.org/srcML/src" revision="REVISION" url="test">

	<unit xmlns:cpp="http://www.srcML.org/srcML/cpp" revision="REVISION" language="C++" filename="empty.cpp" hash="da39a3ee5e6b4b0d3255bfef95601890afd80709"/>

	</unit>
	STDOUT
createfile "list-empty-zip-bz2.txt" "https://raw.githubusercontent.com/srcML/test-data/master/empty/empty.cpp.zip.bz2"
src2srcml --files-from list-empty-zip-bz2.txt --url="test"
check "$empty_srcml_archive"
rmfile list-empty-zip-bz2.txt

define empty_srcml_archive <<- 'STDOUT'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.srcML.org/srcML/src" revision="REVISION" url="test">

	<unit xmlns:cpp="http://www.srcML.org/srcML/cpp" revision="REVISION" language="C++" filename="empty.cpp" hash="da39a3ee5e6b4b0d3255bfef95601890afd80709"/>

	</unit>
	STDOUT
createfile "list-empty-zip-gz.txt" "https://raw.githubusercontent.com/srcML/test-data/master/empty/empty.cpp.zip.gz"
src2srcml --files-from list-empty-zip-gz.txt --url="test"
check "$empty_srcml_archive"
rmfile list-empty-zip-gz.txt


define empty_srcml_archive <<- 'STDOUT'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.srcML.org/srcML/src" revision="REVISION" url="test">

	<unit xmlns:cpp="http://www.srcML.org/srcML/cpp" revision="REVISION" language="C++" filename="empty.cpp" hash="da39a3ee5e6b4b0d3255bfef95601890afd80709"/>

	</unit>
	STDOUT
createfile "list-empty-zip.txt" "https://raw.githubusercontent.com/srcML/test-data/master/empty/empty.cpp.zip"
src2srcml --files-from list-empty-zip.txt --url="test"
check "$empty_srcml_archive"
rmfile list-empty-zip.txt

define empty_srcml_archive <<- 'STDOUT'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.srcML.org/srcML/src" revision="REVISION" url="test">

	<unit xmlns:cpp="http://www.srcML.org/srcML/cpp" revision="REVISION" language="C++" filename="empty.cpp" hash="da39a3ee5e6b4b0d3255bfef95601890afd80709"/>

	</unit>
	STDOUT

createfile "list-empty-cpio.txt" "https://raw.githubusercontent.com/srcML/test-data/master/empty/empty.cpp.cpio"
src2srcml --files-from list-empty-cpio.txt --url="test"
check "$empty_srcml_archive"
rmfile list-empty-cpio.txt

define empty_srcml_archive <<- 'STDOUT'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.srcML.org/srcML/src" revision="REVISION" url="test">

	<unit xmlns:cpp="http://www.srcML.org/srcML/cpp" revision="REVISION" language="C++" filename="empty.cpp" hash="da39a3ee5e6b4b0d3255bfef95601890afd80709"/>

	</unit>
	STDOUT
createfile "list-empty-tar.txt" "https://raw.githubusercontent.com/srcML/test-data/master/empty/empty.cpp.tar"
src2srcml --files-from list-empty-tar.txt --url="test"
check "$empty_srcml_archive"
rmfile list-empty-tar.txt