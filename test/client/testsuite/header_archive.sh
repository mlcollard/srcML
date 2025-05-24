#!/bin/bash
# SPDX-License-Identifier: GPL-3.0-only
#
# @file header_archive.sh
#
# @copyright Copyright (C) 2025 srcML, LLC. (www.srcML.org)

# test framework
source $(dirname "$0")/framework_test.sh

defineXML headerXML <<- 'OUTPUT'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.srcML.org/srcML/src" revision="1.0.0">

	<unit revision="1.0.0" language="C++" filename="n.cpp" hash="2b22284231f33eb19e66388951726a07ccbec135"><decl_stmt><decl><type><name>int</name></type> <name>n</name> <init>= <expr><literal type="number">0</literal></expr></init></decl>;</decl_stmt>
	</unit>

	<unit revision="1.0.0" language="C++" filename="m.cpp" hash="4e505f4b4ab0455bc5357bfe34ddd8430a71d66a"><decl_stmt><decl><type><name>int</name></type> <name>m</name> <init>= <expr><literal type="number">0</literal></expr></init></decl>;</decl_stmt>
	</unit>

	</unit>
OUTPUT
createfile project.xml "$headerXML"

define file1 <<-'EOF'
	---
	xmlns: "http://www.srcML.org/srcML/src"
	language: "C++"
	filename: "n.cpp"
	---
	int n = 0;
EOF

define file2 <<-'EOF'
	---
	xmlns: "http://www.srcML.org/srcML/src"
	language: "C++"
	filename: "m.cpp"
	---
	int m = 0;
EOF

echo -n "$file1" > project.txt
printf '\0' >> project.txt
echo -n "$file2" >> project.txt

srcml project.xml --header
check project.txt

srcml --header project.txt
check project.xml

cat project.txt | srcml
check project.xml

cat project.txt | srcml | srcml --header
check project.txt

defineXML headerCustomXML <<- 'OUTPUT'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.srcML.org/srcML/src" revision="1.0.0">

	<unit xmlns:metrics="http://www.srcML.org/srcML/metrics" revision="1.0.0" language="C++" filename="n.cpp" hash="2b22284231f33eb19e66388951726a07ccbec135"><decl_stmt><decl><type><name>int</name></type> <name>n</name> <init>= <expr><literal type="number">0</literal></expr></init></decl>;</decl_stmt>
	</unit>

	<unit revision="1.0.0" language="C++" filename="m.cpp" hash="4e505f4b4ab0455bc5357bfe34ddd8430a71d66a"><decl_stmt><decl><type><name>int</name></type> <name>m</name> <init>= <expr><literal type="number">0</literal></expr></init></decl>;</decl_stmt>
	</unit>

	</unit>
OUTPUT
createfile projectCustom.xml "$headerCustomXML"

define file1Custom <<-'EOF'
	---
	xmlns: "http://www.srcML.org/srcML/src"
	"xmlns:metrics": "http://www.srcML.org/srcML/metrics"
	language: "C++"
	filename: "n.cpp"
	---
	int n = 0;
EOF

define file2Custom <<-'EOF'
	---
	xmlns: "http://www.srcML.org/srcML/src"
	language: "C++"
	filename: "m.cpp"
	---
	int m = 0;
EOF

echo -n "$file1Custom" > projectCustom.txt
printf '\0' >> projectCustom.txt
echo -n "$file2Custom" >> projectCustom.txt

srcml projectCustom.xml --header
check projectCustom.txt

srcml --header projectCustom.txt
check projectCustom.xml

cat projectCustom.txt | srcml
check projectCustom.xml

cat projectCustom.txt | srcml | srcml --header
check projectCustom.txt

defineXML headerCustomAttributeXML <<- 'OUTPUT'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.srcML.org/srcML/src" revision="1.0.0">

	<unit xmlns:metrics="http://www.srcML.org/srcML/metrics" revision="1.0.0" language="C++" filename="n.cpp" hash="2b22284231f33eb19e66388951726a07ccbec135" metrics:complexity="n"><decl_stmt><decl><type><name>int</name></type> <name>n</name> <init>= <expr><literal type="number">0</literal></expr></init></decl>;</decl_stmt>
	</unit>

	<unit revision="1.0.0" language="C++" filename="m.cpp" hash="4e505f4b4ab0455bc5357bfe34ddd8430a71d66a"><decl_stmt><decl><type><name>int</name></type> <name>m</name> <init>= <expr><literal type="number">0</literal></expr></init></decl>;</decl_stmt>
	</unit>

	</unit>
OUTPUT
createfile projectCustomAttribute.xml "$headerCustomAttributeXML"

define file1CustomAttribute <<-'EOF'
	---
	xmlns: "http://www.srcML.org/srcML/src"
	"xmlns:metrics": "http://www.srcML.org/srcML/metrics"
	language: "C++"
	filename: "n.cpp"
	"metrics:complexity": "n"
	---
	int n = 0;
EOF

define file2CustomAttribute <<-'EOF'
	---
	xmlns: "http://www.srcML.org/srcML/src"
	language: "C++"
	filename: "m.cpp"
	---
	int m = 0;
EOF

echo -n "$file1CustomAttribute" > projectCustomAttribute.txt
printf '\0' >> projectCustomAttribute.txt
echo -n "$file2CustomAttribute" >> projectCustomAttribute.txt

srcml projectCustomAttribute.xml --header
check projectCustomAttribute.txt

srcml --header projectCustomAttribute.txt
check projectCustomAttribute.xml

cat projectCustomAttribute.txt | srcml
check projectCustomAttribute.xml

cat projectCustomAttribute.txt | srcml | srcml --header
check projectCustomAttribute.txt

defineXML headerCustomAttributeAttributeXML <<- 'OUTPUT'
	<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
	<unit xmlns="http://www.srcML.org/srcML/src" revision="1.0.0">

	<unit xmlns:metrics="http://www.srcML.org/srcML/metrics" revision="1.0.0" language="C++" filename="n.cpp" hash="2b22284231f33eb19e66388951726a07ccbec135" metrics:complexity="n"><decl_stmt><decl><type><name>int</name></type> <name>n</name> <init>= <expr><literal type="number">0</literal></expr></init></decl>;</decl_stmt>
	</unit>

	<unit xmlns:slicing="http://www.srcML.org/srcML/slicing" revision="1.0.0" language="C++" filename="m.cpp" hash="4e505f4b4ab0455bc5357bfe34ddd8430a71d66a" slicing:slice="ABCDEGH"><decl_stmt><decl><type><name>int</name></type> <name>m</name> <init>= <expr><literal type="number">0</literal></expr></init></decl>;</decl_stmt>
	</unit>

	</unit>
OUTPUT
createfile projectCustomAttributeAttribute.xml "$headerCustomAttributeAttributeXML"

define file1CustomAttributeAttribute <<-'EOF'
	---
	xmlns: "http://www.srcML.org/srcML/src"
	"xmlns:metrics": "http://www.srcML.org/srcML/metrics"
	language: "C++"
	filename: "n.cpp"
	"metrics:complexity": "n"
	---
	int n = 0;
EOF

define file2CustomAttributeAttribute <<-'EOF'
	---
	xmlns: "http://www.srcML.org/srcML/src"
	"xmlns:slicing": "http://www.srcML.org/srcML/slicing"
	language: "C++"
	filename: "m.cpp"
	"slicing:slice": "ABCDEGH"
	---
	int m = 0;
EOF

echo -n "$file1CustomAttributeAttribute" > projectCustomAttributeAttribute.txt
printf '\0' >> projectCustomAttributeAttribute.txt
echo -n "$file2CustomAttributeAttribute" >> projectCustomAttributeAttribute.txt

srcml projectCustomAttributeAttribute.xml --header
check projectCustomAttributeAttribute.txt

srcml --header projectCustomAttributeAttribute.txt
check projectCustomAttributeAttribute.xml

cat projectCustomAttributeAttribute.txt | srcml
check projectCustomAttributeAttribute.xml

cat projectCustomAttributeAttribute.txt | srcml | srcml --header
check projectCustomAttributeAttribute.txt
