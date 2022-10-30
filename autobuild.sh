#!/bin/bash

BuildDir=./build/
BinDir=./bin/

set -x

rm -rf $BuildDir/*

if [ ! -d "$BuildDir" ];then
    mkdir $BuildDir
fi

if [ ! -d "$BinDir" ];then
    mkdir $BinDir
fi

cd $BuildDir &&
  cmake .. &&
  make
