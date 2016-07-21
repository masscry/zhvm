#!/bin/bash

mkdir -p ./stage

cd ./stage

cmake ../

make -j4

make install
