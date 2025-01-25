#define MAX_DRIVES 20
#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <stdexcept>
#include "Partition/Partition.h"
#include "FileSystem/FileSystem.h"

struct DriveInfo {
    std::string name;
    std::string size;
    std::string model;
};

// Function to list drives using lsblk and store them in a vector
std::vector<DriveInfo> listDrives() {
    std::vector<DriveInfo> drives;
    FILE* fp = popen("lsblk -d -o NAME,SIZE,MODEL --noheadings", "r");
    if (fp == nullptr) {
        throw std::runtime_error("Failed to list drives.");
    }

    char name[128], size[128], model[128];
    while (fscanf(fp, "%s %s %[^\n]", name, size, model) != EOF) {
        drives.push_back({name, size, model});
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
