#include <stdio.h>
#include <string.h>
#include <err.h>
#include <blkid/blkid.h>
#include "Partition/Partition.h"
#include <FileSystem/FileSystem.h>


int main (int argc, char *argv[]) {
   return FormatToSpecifiedFileSystem("/dev/sdb", "ext3");
}