#include <iostream>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <cstdio>
std::string get_mount_point(const char *device)
{
    std::string command = "findmnt -n -o TARGET --source " + std::string(device);
    FILE *pipe = popen(command.c_str(), "r");
    if (!pipe)
    {
        return "";
    }
    char buffer[256];
    std::string mount_point = "";
    if (fgets(buffer, sizeof(buffer), pipe) != NULL)
    {
        mount_point = buffer;

        mount_point.erase(mount_point.find_last_not_of(" \n\r\t") + 1);
    }
    pclose(pipe);
    return mount_point;
}
void unmount_device(const char *device)
{
    std::string command = "sudo umount " + std::string(device);
    std::cout << "Executing: " << command << std::endl;
    system(command.c_str());
}
void mount_device(const char *device, const std::string &mount_point)
{
    if (!std::filesystem::exists(mount_point))
    {
        std::filesystem::create_directories(mount_point);
    }

    std::string command = "sudo mount " + std::string(device) + " " + mount_point;
    std::cout << "Executing: " << command << std::endl;
    int result = system(command.c_str());
    if (result != 0)
    {
        std::cerr << "Error: Failed to mount the device " << device << " to " << mount_point << std::endl;
    }
}

void FormatToSpecifiedFileSystem(const char *device, const char *fileSystem, const char *label)
{
    if (strncmp(device, "/dev/", 5) != 0)
    {
        std::cerr << "Not Permissible: Device path must start with /dev/" << std::endl;
        return;
    }

    if (!std::filesystem::exists(device) || !std::filesystem::is_block_file(device))
    {
        std::cerr << "Invalid Path: The specified device is not a valid block file." << std::endl;
        return;
    }

    std::string mount_point = get_mount_point(device);
    bool was_mounted = !mount_point.empty();

    if (was_mounted)
    {
        std::cout << "Device " << device << " is mounted at " << mount_point << ". Attempting to unmount..." << std::endl;
        unmount_device(device);

        if (!get_mount_point(device).empty())
        {
            std::cerr << "Failed to unmount " << device << ". Please unmount it manually and try again." << std::endl;
            return;
        }
        std::cout << "Device unmounted successfully." << std::endl;
    }

    std::string format_command = "sudo mkfs -t " + std::string(fileSystem) +
                                 " -L " + std::string(label) +
                                 " " + std::string(device);

    std::cout << "Executing: " << format_command << std::endl;
    int result = system(format_command.c_str());

    if (result != 0)
    {
        std::cerr << "Error: Failed to format the device " << device << std::endl;

        if (was_mounted)
        {
            std::cout << "Formatting failed. Attempting to mount the device back to its original location..." << std::endl;
            mount_device(device, mount_point);
        }
        return;
    }

    std::cout << "Successfully formatted " << device << "." << std::endl;

    if (was_mounted)
    {
        std::cout << "Attempting to mount the device back to " << mount_point << "..." << std::endl;
        mount_device(device, mount_point);
        std::cout << "Device mounted successfully." << std::endl;
    }
}