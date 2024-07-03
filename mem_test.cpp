#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#include <iostream>
#include <vector>
#include <cstdio>
#include <CL/err_code.h>   

const char* kernelSource = R"(
__kernel void vram_test(__global int* vram, int size) {
    int id = get_global_id(0);
    if (id < size) {
        vram[id] = id;
    }
}

__kernel void vram_check(__global int* vram, __global int* errors, int size) {
    int id = get_global_id(0);
    if (id < size) {
        if (vram[id] != id) {
            errors[id] = id;
        } else {
            errors[id] = -1;
        }
    }
}
)";

int main() {
    // Select the OpenCL platform
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    cl::Platform platform = platforms.front();

    // Select the OpenCL device
    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
    cl::Device device = devices.front();

    // Create an OpenCL context and queue
    cl::Context context(device);
    cl::CommandQueue queue(context, device);

    // Create the program and kernel
    cl::Program program(context, kernelSource);
    program.build("-cl-std=CL1.2");

    cl::Kernel kernelWrite(program, "vram_test");
    cl::Kernel kernelCheck(program, "vram_check");

    // Allocate memory on the host and device
    int size = 1024 * 1024; // 1 million elements
    std::vector<int> vram(size);
    std::vector<int> errors(size);

    cl::Buffer vramBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int) * size, vram.data());
    cl::Buffer errorBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int) * size, errors.data());

    // Set kernel arguments
    kernelWrite.setArg(0, vramBuffer);
    kernelWrite.setArg(1, size);

    kernelCheck.setArg(0, vramBuffer);
    kernelCheck.setArg(1, errorBuffer);
    kernelCheck.setArg(2, size);

    // Execute the write kernel
    queue.enqueueNDRangeKernel(kernelWrite, cl::NullRange, cl::NDRange(size), cl::NullRange);
    queue.finish();

    // Execute the check kernel
    queue.enqueueNDRangeKernel(kernelCheck, cl::NullRange, cl::NDRange(size), cl::NullRange);
    queue.finish();

    // Read the results back to the host
    queue.enqueueReadBuffer(errorBuffer, CL_TRUE, 0, sizeof(int) * size, errors.data());

    // Check for errors
    bool hasErrors = false;
    for (int i = 0; i < size; ++i) {
        if (errors[i] != -1) {
            std::cout << "Error at index " << i << ": expected " << i << ", found " << errors[i] << std::endl;
            hasErrors = true;
        }
    }

    if (!hasErrors) {
        std::cout << "VRAM test passed successfully!" << std::endl;
    } else {
        std::cout << "VRAM test failed!" << std::endl;
    }

    return 0;
}
