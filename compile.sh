#!/bin/sh

if [ 0"$CC" = "0" ]; then
    export CC=gcc
fi

if [ 0"$CXX" = "0" ]; then
    export CXX=g++
fi

$CXX gobang.cpp -o gobang -Ofast
$CC -c exec.c -o exec.o -O2
$CXX exec.o main.cpp -o main -O2