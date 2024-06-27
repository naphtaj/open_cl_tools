#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#include <iostream>
#include <vector>
#include <cstdio>   

int main() {
    std::vector<cl::Platform> platforms; //vector of cl platforms
    cl::Platform::get(&platforms);
    if (platforms.empty()) {
        std::cerr << "OpenCL platforms not found." << std::endl;
        return 1;
    }
    cl::Platform platform = platforms.front();
    std::cout << "Using platform: " << platform.getInfo<CL_PLATFORM_NAME>() << std::endl;

    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
    if (devices.empty()) {
        std::cerr << "OpenCL devices not found." << std::endl;
        return 1;
    }
    cl::Device device = devices.front();
    std::cout << "Using device: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;

    cl::Context context(device);
    cl::CommandQueue queue(context, device);

    const char* kernelSource =
        "__kernel void hello() {\n"
        "  printf(\"Hello, World!\\n\");\n"
        "}\n";
    cl::Program::Sources sources(1, std::make_pair(kernelSource, strlen(kernelSource)));
    cl::Program program(context, sources);
    program.build("-cl-std=CL1.2");
    cl::Kernel kernel(program, "hello");
    queue.enqueueTask(kernel);
    queue.finish();
    return 0;
}