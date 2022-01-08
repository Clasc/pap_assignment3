#define CL_TARGET_OPENCL_VERSION 200
#include<CL/cl.hpp>
#include<stdio.h>
#include<vector>

void run_parallel() {
    cl_int err;
    cl_uint numPlatforms;

    err = clGetPlatformIDs(0, NULL, &numPlatforms);
    if (err != CL_SUCCESS) {
        printf("\nError calling clGetPlatformIDs. Error code: %d", err);
        return;
    }

    printf("\nDetected OpenCL platforms: %d \n", numPlatforms);
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    auto platform = platforms.front();

    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
    auto device = devices.front();

    auto cl_code = readClFile("parallel.cl");
    cl_context context;
    std::vector<cl_device_type> device;
    getDevices
}

// returns the opencl code in a specific file as a string
std::string readClFile(std::string filename) {
    std::ifstream file(filename);
    return std::string src(std::istreambuf_iterator<char>(file), (std::istreambuf_iterator<char>()));
}
