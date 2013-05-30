#!/bin/bash

mkdir -p .deps
cd .deps
cmake ../../../..
make
cd ..
