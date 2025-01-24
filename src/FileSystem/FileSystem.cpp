#include <iostream>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <unistd.h>
#include <Constants.h>

int FormatToSpecifiedFileSystem(const std::string& device, const std::string& fileSystem) {
    if (device.compare(0, 4, "/dev") != 0) {
        return E_PATHNOTVALID;
    }

    if (!std::filesystem::exists(device) || !std::filesystem::is_block_file(device)) {
        return E_NOTBLOCKPATH;
    }

    std::string command = "sudo mkfs." + fileSystem + " " + device;
    
    int result = system(command.c_str());
    if (result != 0) {
        std::cerr << "Error: Failed to format the device " << device << std::endl;
        return E_FORMATFAILED;
    }

    return 0;
}
