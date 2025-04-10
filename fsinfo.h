#ifndef FSINFO_H
#define FSINFO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void detectMBRFS(FILE *fptr, int *sectorSize);
char* binarySearchFSTypes(short low, short high, unsigned char partitionID);

#define NUM_FS_TYPES 39

typedef struct {
	unsigned char key;
	char value[64];
} fstype;

#endif