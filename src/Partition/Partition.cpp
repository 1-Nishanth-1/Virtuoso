#include <stdio.h>
#include <string.h>
#include <err.h>
#include <blkid/blkid.h>
#include "Partition.h"
#include <iostream>
#include <vector>

int DisplayPartitions(const char *path, std::vector<PartionInfo> *partInfo)
{
   blkid_partlist list;
   blkid_probe pr = blkid_new_probe_from_filename(path);
   list = blkid_probe_get_partitions(pr);
   if (!list)
   {
      err(1, "Failed to open %s", path);
   }
   blkid_partlist ls;
   int nparts, i;

   ls = blkid_probe_get_partitions(pr);

   nparts = blkid_partlist_numof_partitions(ls);
   printf("Number of partitions:%d\n", nparts);

   if (nparts <= 0)
   {
      printf("Please enter correct device name! e.g. \"/dev/sdc\"\n");
      return 1;
   }

   const char *uuid;
   const char *label;
   const char *type;

   PartionInfo pInfo;

   int j = 1;

   if (strncmp(path, "/dev/nvme", 9) == 0)
   {
      for (i = 0; i < nparts; i++)
      {
         char dev_name[20];

         sprintf(dev_name, "%sp%d", path, (i + 1));

         pr = blkid_new_probe_from_filename(dev_name);
         if (!pr)
         {
            nparts++;
            continue;
         }

         blkid_do_probe(pr);
         strcpy(pInfo.name, dev_name);

         blkid_probe_lookup_value(pr, "UUID", &uuid, NULL);
         strcpy(pInfo.uuid, uuid);

         blkid_probe_lookup_value(pr, "LABEL", &label, NULL);
         strcpy(pInfo.label, label);

         blkid_probe_lookup_value(pr, "TYPE", &type, NULL);
         strcpy(pInfo.type, type);
         (*partInfo).push_back(pInfo);
         printf("%d. Name=%s, UUID=%s, LABEL=%s, TYPE=%s\n", j, dev_name, uuid, label, type);
         j++;
      }
   }
   else
   {
      for (i = 0; i < nparts; i++)
      {
         char dev_name[20];

         sprintf(dev_name, "%s%d", path, (i + 1));

         pr = blkid_new_probe_from_filename(dev_name);
         if (!pr)
         {
            nparts++;
            continue;
         }
         blkid_do_probe(pr);

         strcpy(pInfo.name, dev_name);

         blkid_probe_lookup_value(pr, "UUID", &uuid, NULL);
         strcpy(pInfo.uuid, uuid);

         blkid_probe_lookup_value(pr, "LABEL", &label, NULL);
         strcpy(pInfo.label, label);

         blkid_probe_lookup_value(pr, "TYPE", &type, NULL);
         strcpy(pInfo.type, type);

         (*partInfo).push_back(pInfo);
         printf("%d. Name=%s, UUID=%s, LABEL=%s, TYPE=%s\n", j, dev_name, uuid, label, type);
         j++;
      }
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

int PartitionDisk(const char *path, const char *flag, int size)
{
   try
   {

      // \"o\nn\np\n\n\n\ny\nw\n\"
      std::string partition_command =
          "echo -e \"\nn\np\n\n\n +" + std::to_string(size) + flag +
          "\ny\nw\n\" | sudo fdisk " + std::string(path);

      std::cout << "Executing command: " << partition_command << std::endl;

      int result = system(partition_command.c_str());
      if (result != 0)
      {
         std::cerr << "Error: Failed to partition the disk." << std::endl;
         return -1;
      }
      std::string fileSystem;
      std::cin >> fileSystem;

      std::string label;
      std::cin >> label;

      std::string command = "sudo mkfs -t " + std::string(fileSystem) +
                            " -L " + std::string(label) +
                            " " + std::string(path) + "4";
      result = system(command.c_str());
      if (result != 0)
      {
         std::cerr << "Error: Failed to format the disk!" << std::endl;
         return -1;
      }

      return 0;
   }
   catch (const std::exception &e)
   {
      std::cerr << "Exception: " << e.what() << std::endl;
      return -1;
   }
}
