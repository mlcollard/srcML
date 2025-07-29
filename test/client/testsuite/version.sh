#!/bin/bash
# SPDX-License-Identifier: GPL-3.0-only
#
# @file version.sh
#
# @copyright Copyright (C) 2013-2024 srcML, LLC. (www.srcML.org)

# test framework
source $(dirname "$0")/framework_test.sh

# test
define output <<- 'STDOUT'
	srcml C: 1.0.0
	srcml C++: 1.0.0
	srcml C#: 1.0.0
	srcml Java: 1.0.0
	srcml Python: 1.0.0
	srcml Objective-C: 1.0.0
	srcml client: 1.0.0
	libsrcml: 1.0.0
STDOUT

srcml -V
check "$output"

srcml --version
check "$output"
