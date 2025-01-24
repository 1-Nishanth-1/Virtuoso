#include <iostream>
#include <cstdio>
#include <cstring>
#include <err.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <Constants.h>

int BootableUSBCreation(const char* usb_path) {
    if(strncmp(usb_path, "/dev", 4) != 0) {
        return E_PATHNOTVALID;
    }

    if(!!std::filesystem::exist(usb_path) || std::filesystem::is_block_file(usb_path)) {
        return E_NOTBLOCKPATH;
    }

    const std::string unmount_command = "umount " + std::string(usb_path);
    system(unmount_command.c_str());

    return 0;
}
