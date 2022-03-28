#!/bin/bash

cd `dirname $0`
FCC="flatc -c -o ../include --cpp-std c++17"

$FCC relations.fbs
