#!/bin/bash

g++ -o etana etana.cc `root-config --cflags --glibs` -g -Wall -Wextra
