#!/bin/sh

cd ./extern/antlr4
export MAVEN_OPTS="-Xmx1G"
mvn -DskipTests install

cd ./runtime/Cpp 
rm -rf build 
rm -rf run 
mkdir build 
mkdir run
cmake .
make
sudo make install
