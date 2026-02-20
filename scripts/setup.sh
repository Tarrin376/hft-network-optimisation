#!/bin/bash

export CXX=g++
export NUM_PROC=16
export OMP_NUM_THREADS=16

if [ ! -d build ]; then
    mkdir build
    CXX=$CXX cmake -S . -B build -D BUILD_DEPS:BOOL=ON
    cmake --build build -j$NUM_PROC
fi
