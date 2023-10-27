#!/bin/sh

if [ 0"$CC" = "0" ]; then
    export CC=gcc
fi

if [ 0"$CXX" = "0" ]; then
    export CXX=g++
fi

if [ 0"$DEBUG" != "0" ]; then
    export OPTIMIZE="-g"
else
    export OPTIMIZE="-Ofast"
fi

export CFLAGS="$OPTIMIZE "
export CXXFLAGS="$OPTIMIZE --std=c++17"

# Executables
$CXX gobang.cpp -o gobang $CXXFLAGS
$CC -c exec.c -o exec.o $CFLAGS
$CXX exec.o main.cpp -o main $CXXFLAGS

# Tests
$CXX gridTest.cpp -o gridTest $CXXFLAGS
