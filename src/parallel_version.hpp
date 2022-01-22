#define CL_TARGET_OPENCL_VERSION 200
#include<CL/cl.h>
#include<stdio.h>
#include<vector>
#include<iostream>

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
"__kernel void calculatMatrix(__global float *A_in) {\n"
"  const int i = get_global_id(0);\n"
"  const int j = get_global_id(1);\n"
"  if (i < 1 || i == get_global_size(0) - 1) {\n"
"    return;\n"
"  }\n"
"  for (int t = 0; t < 24; t++) {\n"
"    A_in[index_at(i, j, 1)] +=\n"
"        1 / sqrt(A_in[index_at(i + 1, j, 0)] + A_in[index_at(i - 1, j, 2)]);\n"
"  }\n"
"}\n"
};


struct clConfig {
    cl_platform_id platform_id;
    cl_uint num_platforms;
    cl_device_id device_id;
    cl_uint num_devices;
    cl_context context;
    cl_command_queue command_queue;
};

struct myBuff {
    float* buffer;
    size_t size;
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


double runCalculation(const clConfig& config, cl_program& program, const size_t m, const size_t n, const size_t p, myBuff& matrix) {
    auto kernel = myloadKernel(program, "calculatMatrix");

    //i needs to iterate from 1 to m-1, so we need to decrease it by 1
    // also we need to offset by one bc the first row needs to be skipped!
    // this is similar to :
    // for (int i = 1; i < m - 1; i++) 
    const size_t global_offset[2] = { 0,0 };
    const size_t global_worksize[2] = { m , n };

    // printf("offset  = %i \n", global_offset[0]);
    // printf("new m  = %i \n", global_worksize[0]);
    // printf("buffer size out = %i \n", matrix.size);

    cl_int ret;
    auto memobj = clCreateBuffer(config.context, CL_MEM_COPY_HOST_PTR, matrix.size, matrix.buffer, &ret);
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), &memobj);
    if (ret != CL_SUCCESS) {
        printf("something went wrong setting the kernel arguments!  %i", ret);
        throw runtime_error("cl error");
    }

    auto parallelExec = measure([&] {
        ret = clEnqueueNDRangeKernel(config.command_queue, kernel, 2, global_offset, global_worksize, NULL, 0, NULL, NULL);
        // ret = clEnqueueTask(command_queue, kernel, 0, NULL, NULL);
        if (ret != CL_SUCCESS) {
            printf("something went wrong at enquerange!  %i", ret);
            throw runtime_error("cl error");
        }

        ret = clEnqueueReadBuffer(config.command_queue, memobj, CL_TRUE, 0,
            matrix.size, matrix.buffer, 0, NULL, NULL);
        });


    if (ret != CL_SUCCESS) {
        printf("something went wrong at enqread!  %i", ret);
        throw runtime_error("cl error");
    }

    ret = clReleaseKernel(kernel);
    ret = clReleaseMemObject(memobj);
    return parallelExec;
}


void runSetmatrixKernel(const clConfig& config, cl_program& program, const size_t m, const size_t n, const size_t p, myBuff& buff) {
    auto kernel = myloadKernel(program, "setMatrix");

    // printf("buffer size = %i \n", buff.size);
    cl_int ret;
    auto memory = clCreateBuffer(config.context, CL_MEM_READ_WRITE, buff.size, NULL, &ret);
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), &memory);

    const size_t global_worksize[2] = { m , n };
    ret = clEnqueueNDRangeKernel(config.command_queue, kernel, 2, NULL, global_worksize, NULL, 0, NULL, NULL);
    // ret = clEnqueueTask(command_queue, kernel, 0, NULL, NULL);

    ret = clEnqueueReadBuffer(config.command_queue, memory, CL_TRUE, 0,
        buff.size, buff.buffer, 0, NULL, NULL);
    ret = clReleaseKernel(kernel);
    ret = clReleaseMemObject(memory);
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

// pass sizes for dimensions and returns time run
double run_parallel() {
    try {
        tryInitOpenCL();
    }
    catch (const std::exception& e) {
        std::cout << "-1" << std::endl;
        std::cerr << e.what() << '\n';
        return-1;
    }

    auto config = createContext();

    auto program = createProgram(source, config);
    auto setMatrix = (myBuff){
        buff,
        sizeof(buff)
    };

    double time = -1;
    try {
        runSetmatrixKernel(config, program, m, n, p, setMatrix);
        time = runCalculation(config, program, m, n, p, setMatrix);
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }

    // safeImage("matrix_parallel.txt", setMatrix.buffer, m, n, p);
    cleanup(config, program);
    return time;
}