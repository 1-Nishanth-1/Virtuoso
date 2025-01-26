#include <iostream>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <unistd.h>
#include <cstring>

void FormatToSpecifiedFileSystem(const char* device, const char* fileSystem, const char * label) {
    if (strncmp(device, "/dev/", 5) != 0) {
        std::cerr << "Not Permissible" << std::endl;
        return;
    }

    if (!std::filesystem::exists(device) || !std::filesystem::is_block_file(device))
     {
        std::cerr << "Invalid Path" << std::endl;
        return;
    }

    std::string command = "sudo mkfs -t " + std::string(fileSystem) +
                            " -L " + std::string(label) +
                            " " + std::string(device);
    
    int result = system(command.c_str());
    if (result != 0) {
        std::cerr << "Error: Failed to format the device " << device << std::endl;
        return;
    }

    return ;
}
