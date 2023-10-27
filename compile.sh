#!/bin/sh

if [ 0"$CC" = "0" ]; then
    export CC=gcc
fi

if [ 0"$CXX" = "0" ]; then
    export CXX=g++
fi

export CFLAGS=""
export CXXFLAGS="--std=c++17"

# Executables
$CXX gobang.cpp -o gobang -Ofast $CXXFLAGS
$CC -c exec.c -o exec.o -O2 $CFLAGS
$CXX exec.o main.cpp -o main -O2 $CXXFLAGS

# Tests
$CXX gridTest.cpp -o gridTest -g $CXXFLAGS
