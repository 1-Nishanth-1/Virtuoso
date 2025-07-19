#include <stdio.h>
#include <string.h>
#include <err.h>
#include <blkid/blkid.h>
#include "Partition.h"
#include <iostream>
#include <vector>
#include <cstdlib>

int DisplayPartitions(const char *path, std::vector<PartionInfo> *partInfo)
{
   blkid_probe pr = blkid_new_probe_from_filename(path);
   if (!pr)
   {
      warn("Failed to open %s", path);
      return 1;
   }

   blkid_partlist list = blkid_probe_get_partitions(pr);
   if (!list)
   {
      warn("Failed to get partitions for %s", path);
      blkid_free_probe(pr);
      return 1;
   }

   int nparts = blkid_partlist_numof_partitions(list);
   printf("Number of partitions:%d\n", nparts);

   if (nparts <= 0)
   {
      printf("No partitions found on %s. Please ensure the device name is correct (e.g. \"/dev/sdc\")\n", path);
      blkid_free_probe(pr);
      return 1;
   }

   for (int i = 0; i < nparts; i++)
   {
      char dev_name[128];
      snprintf(dev_name, sizeof(dev_name), "%s%d", path, (i + 1));
      if (strncmp(path, "/dev/nvme", 9) == 0)
      {
         snprintf(dev_name, sizeof(dev_name), "%sp%d", path, (i + 1));
      }

      blkid_probe part_pr = blkid_new_probe_from_filename(dev_name);
      if (!part_pr)
      {
         continue;
      }

      if (blkid_do_probe(part_pr) == 0)
      {
         PartionInfo pInfo;
         const char *uuid = "";
         const char *label = "";
         const char *type = "";

         strcpy(pInfo.name, dev_name);

         blkid_probe_lookup_value(part_pr, "UUID", &uuid, NULL);
         if (uuid)
            strcpy(pInfo.uuid, uuid);
         else
            strcpy(pInfo.uuid, "N/A");

         blkid_probe_lookup_value(part_pr, "LABEL", &label, NULL);
         if (label)
            strcpy(pInfo.label, label);
         else
            strcpy(pInfo.label, "N/A");

         blkid_probe_lookup_value(part_pr, "TYPE", &type, NULL);
         if (type)
            strcpy(pInfo.type, type);
         else
            strcpy(pInfo.type, "N/A");

         partInfo->push_back(pInfo);
         printf("%d. Name=%s, UUID=%s, LABEL=%s, TYPE=%s\n", i + 1, pInfo.name, pInfo.uuid, pInfo.label, pInfo.type);
      }
      blkid_free_probe(part_pr);
   }

   blkid_free_probe(pr);

   return 0;
}

int PartitionDisk(const char *path, const char *flag, int size)
{
   try
   {
      std::string partition_command =
          "echo -e \"n\np\n\n\n\n+" + std::to_string(size) + flag +
          "\nw\n\" | sudo fdisk " + std::string(path);

      std::cout << "Executing command: " << partition_command << std::endl;

      int result = system(partition_command.c_str());
      if (result != 0)
      {
         std::cerr << "Error: Failed to partition the disk." << std::endl;
         return -1;
      }

      std::cout << "Disk partitioned successfully." << std::endl;
      return 0;
   }
   catch (const std::exception &e)
   {
      std::cerr << "Exception: " << e.what() << std::endl;
      return -1;
   }
}