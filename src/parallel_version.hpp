#define CL_TARGET_OPENCL_VERSION 200
#include<CL/cl.h>
#include<stdio.h>
#include<vector>
#include<iostream>


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

// returns the opencl code in a specific file as a string
std::string readClFile(std::string filename) {
    std::ifstream file(filename);
    return std::string(std::istreambuf_iterator<char>(file), (std::istreambuf_iterator<char>()));
}

cl_platform_id getPlatformId(cl_int* return_code) {
    cl_platform_id platform_id;
    cl_uint num_platforms;
    *return_code = clGetPlatformIDs(1, &platform_id, &num_platforms);
    return platform_id;
}




void run_parallel() {
    try {
        tryInitOpenCL();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return;
    }
    cl_int ret;
    auto platform = getPlatformId(&ret);

    cl_device_id device_id;
    cl_uint num_devices;
    ret = clGetDeviceIDs(platform, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &num_devices);

    auto context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

    auto command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

    auto memory = clCreateBuffer(context, CL_MEM_READ_WRITE, 32 * sizeof(char), NULL, &ret);

    /* Create Kernel Program from the source */
    auto cl_code = readClFile("parallel.cl");
    auto program = clCreateProgramWithSource(context, 1, (const char**)&cl_code, (const size_t*)cl_code.length() + 1, &ret);

    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

    auto kernel = clCreateKernel(program, "test", &ret);

    /* Set OpenCL Kernel Parameters */
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&memory);

    /* Execute OpenCL Kernel */
    ret = clEnqueueTask(command_queue, kernel, 0, NULL, NULL);

    /* Copy results from the memory buffer */
    ret = clEnqueueReadBuffer(command_queue, memory, CL_TRUE, 0,
        32 * sizeof(char), string, 0, NULL, NULL);

    /* Display Result */
    puts(string);

    /* Finalization */
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(memory);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);

    free(source_str);
}

