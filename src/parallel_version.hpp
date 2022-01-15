#define CL_TARGET_OPENCL_VERSION 200
#include<CL/cl.h>
#include<stdio.h>
#include<vector>
#include<iostream>

#define MEM_SIZE (128)
#define MAX_SOURCE_SIZE (4096)

void tryInitOpenCL() {
    cl_int err;
    cl_uint numPlatforms;
    err = clGetPlatformIDs(0, NULL, &numPlatforms);
    if (err != CL_SUCCESS) {
        std::string message("Error calling clGetPlatformIDs. Error code: ");
        throw std::runtime_error(message + std::to_string(err));
    }
    printf("\nDetected OpenCL platforms: %d \n", numPlatforms);
}

// loads kernel source code into a buffer and returns the size of the source string
static const char* source[] = {
"__kernel void hello_world(__global char *string) {\n"
"  string[0] = 'H';\n"
"  string[1] = 'e';\n"
"  string[2] = 'l';\n"
"  string[3] = 'l';\n"
"  string[4] = 'o';\n"
"  string[5] = ',';\n"
"  string[6] = ' ';\n"
"  string[7] = 'W';\n"
"  string[8] = 'o';\n"
"  string[9] = 'r';\n"
"  string[10] = 'l';\n"
"  string[11] = 'd';\n"
"  string[12] = '!';\n"
"  string[13] = '\\0';\n"
"}\n" };


void run_parallel() {
    try {
        tryInitOpenCL();
    }
    catch (const std::exception& e) {
        std::cout << "-1" << std::endl;
        std::cerr << e.what() << '\n';
        return;
    }

    cl_int ret;
    cl_platform_id platform_id;
    cl_uint num_platforms;
    ret = clGetPlatformIDs(1, &platform_id, &num_platforms);

    cl_device_id device_id;
    cl_uint num_devices;
    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &num_devices);

    char message[MEM_SIZE];

    auto context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

    auto command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

    auto memory = clCreateBuffer(context, CL_MEM_READ_WRITE, MEM_SIZE * sizeof(char), NULL, &ret);

    auto program = clCreateProgramWithSource(context, 1, source, NULL, &ret);

    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

    auto kernel = clCreateKernel(program, "hello_world", &ret);

    if (ret) {
        printf("an error occured during loading the source");
        return;
    }

    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&memory);

    ret = clEnqueueTask(command_queue, kernel, 0, NULL, NULL);

    ret = clEnqueueReadBuffer(command_queue, memory, CL_TRUE, 0,
        MEM_SIZE * sizeof(char), message, 0, NULL, NULL);

    puts(message);

    /* Finalization */
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(memory);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);
}

