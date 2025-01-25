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

struct DriveInfo {
    std::string name;
    std::string size;
    std::string model;
};

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
        // Try to parse with 3 fields first
        int count = sscanf(line, "%s %s %[^\n]", name, size, model);

        // If only 2 fields were found, assume model is missing
        if (count == 2) {
            strcpy(model, "Unknown");
        } 
        // If only 1 field was found, assume size and model are missing
        else if (count == 1) {
            strcpy(size, "Unknown");
            strcpy(model, "Unknown");
        }

        // Add to vector
        drives.push_back({name, size, model});

        // Reset values for the next iteration
        strcpy(name, "N/A");
        strcpy(size, "N/A");
        strcpy(model, "N/A");
    }

    pclose(fp);
    return drives;
}


// Function to display available drives with numbering
void displayDrives(const std::vector<DriveInfo>& drives) {
    std::cout << "Available drives:\n";
    for (size_t i = 0; i < drives.size(); ++i) {
        std::cout << i + 1 << ". " << drives[i].name << " (" 
                  << drives[i].size << ", " 
                  << drives[i].model << ")\n";
    }
}

int main() {
    try {
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

        std::string selectedDrive = "/dev/" + drives[choice - 1].name;

        // Call DisplayPartitions with the selected drive
        return DisplayPartitions(selectedDrive.c_str());

    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }

    return 0;
}
