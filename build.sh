#!/bin/bash

g++ *cpp -o test_bin -std=c++11 -O0 && ./test_bin

g++ *cpp -o test_bin -std=c++11 -O3 && ./test_bin
