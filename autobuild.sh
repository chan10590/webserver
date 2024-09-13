#!/bin/bash

set -x

rm -rf `pwd`/log/*
cd `pwd`/build
make clean
rm -rf `pwd`/*
cmake ..
make server
