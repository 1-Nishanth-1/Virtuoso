#define MAX_DRIVES 20
#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include "Partition/Partition.h"
#include "FileSystem/FileSystem.h"
#include "BootableUSBCreation/BootableUSBCreation.h"

struct DriveInfo
{
    std::string name;
    std::string size;
    std::string model;
};

void DisplayMenu()
{
    std::cout << "1. Format Partition\n";
    std::cout << "2. Partition Drive\n";
    std::cout << "3. Create Bootable USB\n";
    std::cout << "4. Exit\n";
}

std::vector<DriveInfo> listDrives()
{
    std::vector<DriveInfo> drives;
    FILE *fp = popen("lsblk -d -o NAME,SIZE,MODEL --noheadings", "r");
    if (fp == nullptr)
    {
        throw std::runtime_error("Failed to list drives.");
    }

    char line[256];
    char name[128] = "N/A";
    char size[128] = "N/A";
    char model[128] = "N/A";

    while (fgets(line, sizeof(line), fp))
    {
        int count = sscanf(line, "%s %s %[^\n]", name, size, model);
        if (count == 2)
        {
            strcpy(model, "Unknown");
        }
        else if (count == 1)
        {
            strcpy(size, "Unknown");
            strcpy(model, "Unknown");
        }
        drives.push_back({name, size, model});

        strcpy(name, "N/A");
        strcpy(size, "N/A");
        strcpy(model, "N/A");
    }

    pclose(fp);
    return drives;
}

void displayDrives(const std::vector<DriveInfo> &drives)
{
    std::cout << "Available drives:\n";
    for (size_t i = 0; i < drives.size(); ++i)
    {
        std::cout << i + 1 << ". " << drives[i].name << " ("
                  << drives[i].size << ", "
                  << drives[i].model << ")\n";
    }
}

int main()
{
    int choice;
    std::string selectedDrive;
    int user_choice;
    try
    {
        DisplayMenu();
        std::cout << "Enter your choice: ";
        std::cin >> user_choice;

        if (user_choice == 4)
        {
            exit(0);
        }
        std::vector<DriveInfo> drives = listDrives();

        if (drives.empty())
        {
            std::cerr << "No drives found.\n";
            return 1;
        }

        displayDrives(drives);

        std::cout << "Select a drive (1-" << drives.size() << "): ";
        std::cin >> choice;

        if (choice < 1 || static_cast<size_t>(choice) > drives.size())
        {
            std::cerr << "Invalid selection.\n";
            return 1;
        }

        selectedDrive = "/dev/" + drives[choice - 1].name;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << "\n";
        return 1;
    }
    std::cout << "You have selected " << selectedDrive.c_str() << std::endl;

    if (user_choice == 3)
    {
        std::string iso_path;

        std::cout << "Enter ISO Path: ";
        std::cin >> iso_path;

        if (iso_path.length() == 0)
        {
            std::cerr << "Invalid Path" << std::endl;
            return 0;
        }
        BootableUSBCreation(selectedDrive.c_str(), iso_path.c_str());
    }
    else if (user_choice == 2)
    {
        std::string flag;
        int size;

        std::cout << "Enter size flag (M for MB, G for GB): ";
        std::cin >> flag;

        std::cout << "Enter partition size: ";
        std::cin >> size;

        PartitionDisk(selectedDrive.c_str(), flag.c_str(), size);
    }
    else if (user_choice == 1)
    {

        std::vector<PartionInfo> partInfo;

        int ret = DisplayPartitions(selectedDrive.c_str(), &partInfo);
        if (ret != 0)
        {
            return 1;
        }

        std::cout << "Select a partition to format (1-" << partInfo.size() << "): ";
        std::cin >> choice;

        if (choice < 1 || static_cast<size_t>(choice) > partInfo.size())
        {
            std::cerr << "Invalid selection.\n";
            return 1;
        }

        std::string selectedPartition = partInfo[choice - 1].name;
        std::cout << "You have selected partition: " << selectedPartition << std::endl;

        std::string label;
        std::string fileSystem;
        std::cout << "Enter File System (e.g., ntfs, ext4): ";
        std::cin >> fileSystem;

        std::cout << "Enter Label: ";
        std::cin >> label;

        FormatToSpecifiedFileSystem(selectedPartition.c_str(), fileSystem.c_str(), label.c_str());
    }

    return 0;
}