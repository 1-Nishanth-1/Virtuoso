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

struct DriveInfo {
    std::string name;
    std::string size;
    std::string model;
};

void DisplayMenu() {
    std::cout << "1. Format Disk\n";
    std::cout << "2. Partition Drive and Label\n";
    std::cout << "3. Create Bootable USB\n";
    std::cout << "4. Exit\n";
}

std::vector<DriveInfo> listDrives() {
    std::vector<DriveInfo> drives;
    FILE* fp = popen("lsblk -d -o NAME,SIZE,MODEL --noheadings", "r");
    if (fp == nullptr) {
        throw std::runtime_error("Failed to list drives.");
    }

    char line[256];  
    char name[128] = "N/A";  
    char size[128] = "N/A";  
    char model[128] = "N/A";  

    while (fgets(line, sizeof(line), fp)) {
        int count = sscanf(line, "%s %s %[^\n]", name, size, model);
        if (count == 2) {
            strcpy(model, "Unknown");
        } 
        else if (count == 1) {
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


void displayDrives(const std::vector<DriveInfo>& drives) {
    std::cout << "Available drives:\n";
    for (size_t i = 0; i < drives.size(); ++i) {
        std::cout << i + 1 << ". " << drives[i].name << " (" 
                  << drives[i].size << ", " 
                  << drives[i].model << ")\n";
    }
}

int main() {
    std::string selectedDrive;
    int user_choice;
    try {
        DisplayMenu();
        std::cout << "Enter your choice: ";
        std::cin >> user_choice;
        
        if(user_choice == 4) {
            exit(0);
        }
        std::vector<DriveInfo> drives = listDrives();

        if (drives.empty()) {
            std::cerr << "No drives found.\n";
            return 1;
        }

        displayDrives(drives);

        int choice;
        std::cout << "Select a drive (1-" << drives.size() << "): ";
        std::cin >> choice;

        if (choice < 1 || static_cast<size_t>(choice) > drives.size()) {
            std::cerr << "Invalid selection.\n";
            return 1;
        }

        selectedDrive = "/dev/" + drives[choice - 1].name;
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    } 
    std::cout<<selectedDrive.c_str();

    if(user_choice == 3) {
        std::string device_path;
        std::string iso_path;

        std::cout << selectedDrive.c_str();

        if (iso_path.length() == 0) {
            std::cerr << "Invalid Path" << std::endl;
            return 0;
        }
        BootableUSBCreation(selectedDrive.c_str(), iso_path.c_str());
    
    } else if (user_choice == 2) {
        std::string path, flag;
        int size;

        std::cout << "Enter the device path (e.g., /dev/sdX): ";
        std::cin >> path;

        std::cout << "Enter partition type flag (e.g., primary, extended, logical): ";
        std::cin >> flag;

        std::cout << "Enter partition size in MB: ";
        // if(path.length == 0 || flag.length == 0) {
        //     std::cerr << "Invalid arguments" << std::endl;
        //     return;
        // }
        PartitionDisk(path.c_str(), flag.c_str(), size);
    }       

   
   return 0;
}
