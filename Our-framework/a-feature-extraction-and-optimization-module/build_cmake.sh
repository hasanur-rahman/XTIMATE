#!/bin/bash

# clear everything
rm -r CMakeCache.txt libextractFeatures.a libobtainCompressionResults.a cmake_install.cmake Makefile CMakeFiles build

cmake .
make
