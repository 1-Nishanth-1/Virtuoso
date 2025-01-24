#include <stdio.h>
#include <string.h>
#include <err.h>
#include <blkid/blkid.h>
#include "Partition/Partition.h"


int main (int argc, char *argv[]) {
   int ret = displayPartitions(argv[1]);
   printf("%d\n",ret);
}