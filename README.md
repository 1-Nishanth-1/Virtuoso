**Linux Tool for Formatting Drives and Creating Bootable Linux USBs**

**Has the following features:**

**1. Disk Formatting**

Allow users to select a hard disk or USB drive and format it to a specified file system (e.g., FAT32, NTFS, ext4).  
Provide options to partition the drive and label partitions.

**2. Bootable USB Creation**

Accept a Linux ISO file as input.  
Write the ISO image to the USB drive, making it bootable.  
Verify the integrity of the written image using checksums.

**3. User-Friendly Interface**

CLI-based interface for usability.  
(To do: Include safety checks to prevent accidental overwrites of system drives.)

**Dependencies**

The tool relies on the library `libblkid`  
Install it on Debian-based systems with the following command:

        sudo apt install libblkid-dev

Compilation

To compile the source code, run `make` in root folder.

Usage

Run the compiled program with administrative privileges:

`sudo ./virtuoso`

Feel free to suggest improvements or submit pull requests for additional features.
