#define CL_TARGET_OPENCL_VERSION 120
#include <CL/opencl.h>

int main(int argc, char const* argv[]) {
    /* code */
    clCreateBuffer(context, CL_MEM_READ_WRITE, size_in_bytes, host_ptr, errcode_ret);
    clBuildProgram(program, num_devices, device_list, options, pfn_notify, user_data_ptr);
    return 0;
}
