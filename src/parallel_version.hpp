#define CL_TARGET_OPENCL_VERSION 200
#include<CL/opencl.h>
#include <stdio.h>

int run_parallel() {
    /* code */
    printf("Hi");
    int* error;
    cl_context context;
    // clCreateBuffer(context, CL_MEM_READ_WRITE, 0, nullptr, error);
    return 0;
}
