#include <iostream>
#include <cstdio>
#include <cstring>
#include <err.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <blkid/blkid.h> 
#include "../Constants/Constants.h"

void DisplayUSB() {
    const char* command = "lsusb";
    system(command);
}

void checkSum() {
    
}

void BootableUSBCreation(const char* usb_path, const char* iso_file) {
    if (strncmp(usb_path, "/dev/", 5) != 0) {
        std::cerr << "Invalid path" << std::endl;
        return;
    }

    if (!std::filesystem::exists(usb_path) || !std::filesystem::is_block_file(usb_path)) {
        std::cerr << "Not a block file" << std::endl;
        return;
    }

    // const std::string unmount_command = "umount " + std::string(usb_path);
    // std::cout << unmount_command << std::endl;
    // int unmount_result = system(unmount_command.c_str());
    // if (unmount_result != 0) {
    //     std::cerr << "Failed to unmount the USB device." << std::endl;
    //     return;
    // }

    const std::string write_command = "sudo dd if=" + std::string(iso_file) + " of=" + std::string(usb_path) + " bs=4M status=progress && sync";
    std::cout << write_command << std::endl;
    int write_result = system(write_command.c_str());
    if (write_result != 0) {
        std::cerr << "Failed to write ISO File" << std::endl;
        return;
    }

    const std::string partition_command = "echo -e \"o\nn\np\n\n\n+512M\ny\nw\n\" | sudo fdisk " + std::string(usb_path);
    
    int partition_result = system(partition_command.c_str());
    if(partition_result != 0) {
        std::cerr << "Failed to create partition" << std::endl;
        return; 
    }



    const std::string mount_command = "mount " + std::string(usb_path) + " /mnt";
    std::cout << mount_command << std::endl;
    // int mount_result = system(mount_command.c_str());
    // if(mount_result != 0) {
    //     std::cerr << "Failed to mount the usb." << std::endl;
    //     return;
    // }
    

    const std::string persistence_config = "echo -e \"/ union\" > /mnt/persistence.conf";
    std::cout << persistence_config << std::endl;
    // int persistence_result = system(persistence_config.c_str());
    // if (persistence_result != 0) {
    //     std::cerr << "Failed to create persistence configuration." << std::endl;
    //     return;
    // }
    std::cout << "Successfully created Bootable USB" << std::endl;
}