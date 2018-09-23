#!/bin/bash

#echo "$1" | g++ -x c++ -S - -o- | grep -Eo "^(_.*:)" | sed -e 's/:$//'
echo "$1" | g++ -x c++ -S - -o-