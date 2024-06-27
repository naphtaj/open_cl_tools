#include <CL/cl.hpp>
#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <thread>

int main() {
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
        // Print platform info
    std::cout << "Using " << platforms.size() << " available hardware\n";
    for (auto &platform : platforms) {
        std::string platformName, platformVendor, platformVersion;
        platform.getInfo(CL_PLATFORM_NAME, &platformName);
        platform.getInfo(CL_PLATFORM_VENDOR, &platformVendor);
        platform.getInfo(CL_PLATFORM_VERSION, &platformVersion);

        std::cout << "Name Platform: " << platformName << '\n';
        std::cout << "Vendor: " << platformVendor << '\n';
        std::cout << "Platform Version: " << platformVersion << '\n';

        std::vector<cl::Device> devices;
        platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);

        for (auto &device : devices) {
            std::string deviceName, deviceVendor, driverVersion;
            cl_uint numCores;
            cl_ulong memSize;

            device.getInfo(CL_DEVICE_NAME, &deviceName);
            device.getInfo(CL_DEVICE_VENDOR, &deviceVendor);
            device.getInfo(CL_DRIVER_VERSION, &driverVersion);
            device.getInfo(CL_DEVICE_MAX_COMPUTE_UNITS, &numCores);
            device.getInfo(CL_DEVICE_GLOBAL_MEM_SIZE, &memSize);

            std::cout << "  Device name: " << deviceName << '\n';
            std::cout << "  Vendor name: " << deviceVendor << '\n';
            std::cout << "  Driver Version: " << driverVersion << '\n';
            std::cout << "  Total Cores: " << numCores << '\n';
            std::cout << "  Memory size: " << memSize / (1024 * 1024) << "MB\n\n";
        }
        std::cout << "-------------------------\n";
    }
    return 0;
}