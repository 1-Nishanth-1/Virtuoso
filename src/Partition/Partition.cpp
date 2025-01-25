#include <stdio.h>
#include <string.h>
#include <err.h>
#include <blkid/blkid.h>
#include "Partition.h"

int DisplayPartitions(const char* path) {
   blkid_partlist list;
   blkid_probe pr = blkid_new_probe_from_filename(path);
   list = blkid_probe_get_partitions(pr);
   if (!list) {
      err(1, "Failed to open %s", path);
   }
   blkid_partlist ls;
   int nparts, i;

   ls = blkid_probe_get_partitions(pr);
   nparts = blkid_partlist_numof_partitions(ls);
   printf("Number of partitions:%d\n", nparts);

   if (nparts <= 0){
      printf("Please enter correct device name! e.g. \"/dev/sdc\"\n");
      return 1;
   }

   const char *uuid;
   const char *label;
   const char *type;


   for (i = 0; i < nparts; i++) {
      char dev_name[20];

      sprintf(dev_name, "%s%d", path, (i+1));

      pr = blkid_new_probe_from_filename(dev_name);
      blkid_do_probe(pr);

      blkid_probe_lookup_value(pr, "UUID", &uuid, NULL);

      blkid_probe_lookup_value(pr, "LABEL", &label, NULL);

      blkid_probe_lookup_value(pr, "TYPE", &type, NULL);

      printf("Name=%s, UUID=%s, LABEL=%s, TYPE=%s\n", dev_name, uuid, label, type);

   }

   blkid_free_probe(pr);

   return 0;
}

// long long getPartitionSize(const char* path) {
//    const char* command = "lsblk -b -o SIZE -n " + std::to_string(path);
//    FILE *fp = popen(command, "r");
   
//    char result[128];
//    if(fgets(result, sizeof(result), fp)) {
//       long long size = atoll(result);
//       fclose(fp);
//       return size;
//    } 
// }

// bool SizeWithinBounds(const char* path, char flag, int size) {
//    long long device_size = getPartitionSize(path);
//    long long user_size;
//    switch (flag)
//    {
//    case 'K':
//       user_size = size * 1024 * 1024;
//       break;
//    case 'M':
//       user_size = size * 1024 * 1024 * 1024;
//       break;

//    case 'G':
//       user_size = size * 1024 * 1024 * 1024 * 1024;
//       break;
//    default:
//       break;
//    }

//    return user_size <= device_size;
// }

// int PartitionDisk(const char* path, const char* flag, int size) {
//    try {
//       std::string partition_command = "echo -e \"o\nn\np\n\n\n+" + std::to_string(size) + std::to_string(flag) + "\nw | sudo fdisk " + std::to_string(path);
//       int result = system(partition_command.c_str());
//       return result;
//    } catch(const std::exception& e) {
//       std::cerr << "Exception: " + e.what() << std::endl;
//    }
// }
