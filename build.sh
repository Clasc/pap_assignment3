#!/usr/bin/bash
g++ -O3 -I /usr/local/cuda-11.4/targets/x86_64-linux/include/ -L /usr/local/cuda-11.4/targets/x86_64-linux/lib/ -o build/pap_assignment3 src/main.cpp -lOpenCL