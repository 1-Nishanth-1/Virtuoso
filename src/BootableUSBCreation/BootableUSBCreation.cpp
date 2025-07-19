#include <iostream>
#include <cstdio>
#include <cstring>
#include <err.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include "../Constants/Constants.h"

void DisplayUSB()
{
    system("lsusb");
}

bool checkSum(const char *iso_path, const char *usb_path)
{
    struct stat file_stat;
    if (stat(iso_path, &file_stat) == -1)
    {
        std::cerr << "Failed to get ISO file details.\n";
        return false;
    }

    long long iso_size = file_stat.st_size;
    char compare_1[256] = {0};
    char compare_2[256] = {0};

    std::string usb_checksum_cmd = "sudo head -c " + std::to_string(iso_size) + " " + std::string(usb_path) + " | sha256sum | awk '{ print $1 }'";
    FILE *fp_usb = popen(usb_checksum_cmd.c_str(), "r");
    if (fp_usb == nullptr)
    {
        std::cerr << "Failed to calculate USB checksum." << std::endl;
        return false;
    }
    fgets(compare_1, sizeof(compare_1), fp_usb);
    pclose(fp_usb);

    std::string iso_checksum_cmd = "sha256sum " + std::string(iso_path) + " | awk '{ print $1 }'";
    FILE *fp_iso = popen(iso_checksum_cmd.c_str(), "r");
    if (fp_iso == nullptr)
    {
        std::cerr << "Failed to calculate ISO checksum." << std::endl;
        return false;
    }
    fgets(compare_2, sizeof(compare_2), fp_iso);
    pclose(fp_iso);

    // Trim newline characters
    compare_1[strcspn(compare_1, "\n")] = 0;
    compare_2[strcspn(compare_2, "\n")] = 0;

    if (strcmp(compare_1, compare_2) == 0)
    {
        std::cout << "Checksum matches. USB is verified.\n";
        return true;
    }
    else
    {
        std::cerr << "Checksum mismatch. USB verification failed.\n";
        std::cerr << "USB Checksum: " << compare_1 << std::endl;
        std::cerr << "ISO Checksum: " << compare_2 << std::endl;
        return false;
    }
}

void BootableUSBCreation(const char *usb_path, const char *iso_file)
{
    if (strncmp(usb_path, "/dev/", 5) != 0)
    {
        std::cerr << "Invalid device path. It should start with /dev/\n";
        return;
    }

    if (!std::filesystem::exists(usb_path) || !std::filesystem::is_block_file(usb_path))
    {
        std::cerr << "The specified path is not a block device.\n";
        return;
    }

    const std::string write_command = "sudo dd if=" + std::string(iso_file) + " of=" + std::string(usb_path) + " bs=4M status=progress && sync";
    std::cout << "Writing ISO to USB. This may take a while..." << std::endl;
    int write_result = system(write_command.c_str());
    if (write_result != 0)
    {
        std::cerr << "Failed to write ISO file to USB.\n";
        return;
    }

    std::cout << "Successfully wrote ISO to USB. Verifying checksum..." << std::endl;

    if (checkSum(iso_file, usb_path))
    {
        std::cout << "Successfully created Bootable USB.\n";
    }
    else
    {
        std::cerr << "Bootable USB creation failed due to checksum mismatch." << std::endl;
    }
}