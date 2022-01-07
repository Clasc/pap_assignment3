#define CL_TARGET_OPENCL_VERSION 200
#include<CL/cl.h>
#include <stdio.h>

void run_parallel() {
    cl_int err;
    cl_uint numPlatforms;

    err = clGetPlatformIDs(0, NULL, &numPlatforms);
    if (err != CL_SUCCESS) {
        printf("\nError calling clGetPlatformIDs. Error code: %d", err);
        return;
    }

    printf("\nDetected OpenCL platforms: %d", numPlatforms);
}
