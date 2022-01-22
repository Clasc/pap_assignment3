#define CL_TARGET_OPENCL_VERSION 200
#include<CL/cl.h>
#include<stdio.h>
#include<vector>
#include<iostream>
#define MAX_SOURCE_SIZE (4096)

static const cl_int m = 4, n = 4, p = 2;
static const size_t MEM_SIZE = m * n * p;

int index_at(int x, int y, int z) {
    return x + n * (y + p * z);
}

void safeImage(const char* filename, float* A) {
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
"int index_at(const int n, const int p, int x, int y, int z) {\n"
"  return x + n * (y + p * z);\n"
"}\n"
"\n"
"__kernel void setMatrix(const int maxRow, const int maxCol, const int maxDepth,\n"
"                        __global float *A) {\n"
"  const int row = get_global_id(0);\n"
"  const int col = get_global_id(1);\n"
"  index_at(maxCol, maxDepth, row, col, 0);\n"
"  A[index_at(maxCol, maxDepth, row, col, 0)] = (float)row / ((float)col + 1.00);\n"
"  A[index_at(maxCol, maxDepth, row, col, 1)] = 1.00;\n"
"  A[index_at(maxCol, maxDepth, row, col, 2)] = (float)col / ((float)row + 1.00);\n"
"}\n"
};

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
    auto context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
    auto command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

    auto program = clCreateProgramWithSource(context, 1, source, NULL, &ret);

    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

    auto kernel = clCreateKernel(program, "setMatrix", &ret);

    if (ret) {
        printf("an error occured during loading the source");
        return;
    }

    float result[MEM_SIZE];
    size_t buffer_size = sizeof(result);
    printf("buffer size = %i \n", buffer_size);
    auto memory = clCreateBuffer(context, CL_MEM_READ_WRITE, buffer_size, NULL, &ret);

    ret = clSetKernelArg(kernel, 0, sizeof(cl_int), &m);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_int), &n);
    ret = clSetKernelArg(kernel, 2, sizeof(cl_int), &p);
    ret = clSetKernelArg(kernel, 3, sizeof(cl_mem), &memory);


    const size_t global_worksize[2] = { m , n };
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, global_worksize, NULL, 0, NULL, NULL);
    // ret = clEnqueueTask(command_queue, kernel, 0, NULL, NULL);

    ret = clEnqueueReadBuffer(command_queue, memory, CL_TRUE, 0,
        buffer_size, result, 0, NULL, NULL);

    /* Finalization */
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(memory);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);

    safeImage("matrix_parallel.txt", result);
}