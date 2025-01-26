#ifndef PARTITION_H
#define PARTITION_H
#include <iostream>
#include <vector>
struct PartionInfo{
    char name[100];
    char uuid[100];
    char label[100];
    char type[100];

};


int DisplayPartitions(const char* path, std::vector<PartionInfo> *partInfo);
int PartitionDisk(const char* path, const char* flag, int size);

#endif
