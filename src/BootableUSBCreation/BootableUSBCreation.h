#ifndef BOOTABLEUSBCREATION_H
#define BOOTABLEUSBCREATION_H 

void DisplayUSB();
void BootableUSBCreation(const char* usb_path, const char* iso_file);
bool checkSum(const char* iso_path, const char* usb_path);

#endif
