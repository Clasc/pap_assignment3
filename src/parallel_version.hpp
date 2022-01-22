#define CL_TARGET_OPENCL_VERSION 200
#include<CL/cl.h>
#include<stdio.h>
#include<vector>
#include<iostream>

void safeImage(const char* filename, float* A, const size_t m, size_t n, size_t p) {
    auto index_at = [&m, &n, &p](int x, int y, int z) {return x + m * (y + n * z);};
    std::ofstream ofs(filename, std::ofstream::out);
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < p; k++) {
                ofs << " " << A[index_at(i, j, k)];
            }
            ofs << std::endl;
        }
    }
    ofs.close();
}

struct clConfig {
    cl_platform_id platform_id;
    cl_uint num_platforms;
    cl_device_id device_id;
    cl_uint num_devices;
    cl_context context;
    cl_command_queue command_queue;
};


clConfig createContext() {
    cl_int ret;
    cl_platform_id platform_id;
    cl_uint num_platforms;
    ret = clGetPlatformIDs(1, &platform_id, &num_platforms);

    cl_device_id device_id;
    cl_uint num_devices;
    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &num_devices);
    auto context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
    auto command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
    if (ret != CL_SUCCESS) {
        std::string message("an error occured during setup of opencl");
        throw std::runtime_error(message + std::to_string(ret));
    }
    return (clConfig) { platform_id, num_platforms, device_id, num_devices, context, command_queue };
}

cl_program createProgram(const char** src, const clConfig& config) {
    cl_int ret;
    auto program = clCreateProgramWithSource(config.context, 1, src, NULL, &ret);
    ret = clBuildProgram(program, 1, &config.device_id, NULL, NULL, NULL);
    if (ret != CL_SUCCESS) {
        std::string message("an error occured during setup of opencl");
        throw std::runtime_error(message + std::to_string(ret));
    }
    return program;
}

cl_kernel myloadKernel(cl_program& program, const char* kernel_name) {
    cl_int ret;
    auto kernel = clCreateKernel(program, kernel_name, &ret);
    if (ret) {
        std::string message("an error occured during loading the source");
        throw std::runtime_error(message + std::to_string(ret));
    }
    return kernel;
}

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

void runSetmatrixKernel(const clConfig& config, cl_program& program, const size_t m, const size_t n, const size_t p) {
    const size_t MEM_SIZE = m * n * p;
    auto kernel = myloadKernel(program, "setMatrix");

    float result[MEM_SIZE];
    size_t buffer_size = sizeof(result);
    printf("buffer size = %i \n", buffer_size);
    cl_int ret;
    auto memory = clCreateBuffer(config.context, CL_MEM_READ_WRITE, buffer_size, NULL, &ret);
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), &memory);

    const size_t global_worksize[2] = { m , n };
    ret = clEnqueueNDRangeKernel(config.command_queue, kernel, 2, NULL, global_worksize, NULL, 0, NULL, NULL);
    // ret = clEnqueueTask(command_queue, kernel, 0, NULL, NULL);

    ret = clEnqueueReadBuffer(config.command_queue, memory, CL_TRUE, 0,
        buffer_size, result, 0, NULL, NULL);
    ret = clReleaseKernel(kernel);
    ret = clReleaseMemObject(memory);
    safeImage("matrix_parallel.txt", result, m, n, p);
}

void cleanup(clConfig& config, cl_program& program) {
    cl_int ret;
    /* Finalization */
    ret = clFlush(config.command_queue);
    ret = clFinish(config.command_queue);
    ret = clReleaseProgram(program);
    ret = clReleaseCommandQueue(config.command_queue);
    ret = clReleaseContext(config.context);
    if (ret != CL_SUCCESS) {
        std::string message("Error cleaning up opencl context and command queue");
        throw std::runtime_error(message + std::to_string(ret));
    }
}

// loads kernel source code into a buffer and returns the size of the source string
static const char* source[] = {
"int index_at(int x, int y, int z) {\n"
"  int m = get_global_size(0);\n"
"  int n = get_global_size(1);\n"
"  return x + m * (y + n * z);\n"
"}\n"
"__kernel void setMatrix(__global float *A) {\n"
"  const int i = get_global_id(0);\n"
"  const int j = get_global_id(1);\n"
"  A[index_at(i, j, 0)] = (float)i / ((float)j + 1.00);\n"
"  A[index_at(i, j, 1)] = 1.00;\n"
"  A[index_at(i, j, 2)] = (float)j / ((float)i + 1.00);\n"
"}\n"
};

// pass sizes for dimensions
void run_parallel(const size_t m, const size_t n, const size_t p) {
    try {
        tryInitOpenCL();
    }
    catch (const std::exception& e) {
        std::cout << "-1" << std::endl;
        std::cerr << e.what() << '\n';
        return;
    }

    auto config = createContext();

    auto program = createProgram(source, config);

    runSetmatrixKernel(config, program, m, n, p);

    cleanup(config, program);
}