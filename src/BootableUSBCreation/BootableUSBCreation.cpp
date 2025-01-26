#include <iostream>
#include <cstdio>
#include <cstring>
#include <err.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <blkid/blkid.h> 
#include <sys/stat.h>

void DisplayUSB() {
    system("lsusb");
}

bool checkSum(const char* iso_path, const char* usb_path) {
    struct stat file_stat;
    if (stat(iso_path, &file_stat) == -1) {
        std::cerr << "Fetch details failed.\n";
        return false;
    }

    long long iso_size = file_stat.st_size;
    char compare_1[256], compare_2[256];

    std::string usb_checksum = "sudo head -c " + std::to_string(iso_size) + " " + std::string(usb_path) + " | sha256sum | awk '{ print $1 }'";
    FILE* fp = popen(usb_checksum.c_str(), "r");
    fgets(compare_1, sizeof(compare_1), fp);

    FILE* fp_1 = popen(("sha256sum " + std::string(iso_path) + " | awk '{ print $1 }'").c_str(), "r");
    fgets(compare_2, sizeof(compare_2), fp_1);

    if (strcmp(compare_1, compare_2) == 0) {
        std::cout << "Checksum matches. USB is verified.\n";
    } else {
        std::cerr << "Checksum mismatch. USB verification failed.\n";
        return false;
    }

    pclose(fp);
    pclose(fp_1);
    return true;
}

void BootableUSBCreation(const char* usb_path, const char* iso_file) {
    if (strncmp(usb_path, "/dev/", 5) != 0) {
        std::cerr << "Invalid path\n";
        return;
    }

    if (!std::filesystem::exists(usb_path) || !std::filesystem::is_block_file(usb_path)) {
        std::cerr << "Not a block file\n";
        return;
    }

    const std::string write_command = "sudo dd if=" + std::string(iso_file) + " of=" + std::string(usb_path) + " bs=4M status=progress && sync";
    int write_result = system(write_command.c_str());
    if (write_result != 0) {
        std::cerr << "Failed to write ISO file\n";
        return;
    }

      std::cout << "Successfully created Bootable USB\n";


    const std::string partition_command = "echo -e \"o\nn\np\n\n\n\ny\nw\n\" | sudo fdisk " + std::string(usb_path);
    if(checkSum(iso_file, usb_path)) {
        int partition_result = system(partition_command.c_str());
        if (partition_result != 0) {
            std::cerr << "Failed to create partition\n";
            return;
        } 
        std::cout << "Partition Successful.\n";
    } else return;
    

    std::cout << "Successfully created Bootable USB\n";
}
