#!/bin/bash
set -x
CC="$1"
echo file1.cpp > list
echo file2.cpp >> list
touch file1.cpp
touch file2.cpp
while read line
do
    echo $line
    $CC -fsyntax-only $line -v
done < list
