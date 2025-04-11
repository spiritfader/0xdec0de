#ifndef FSINFO_H
#define FSINFO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_FS_TYPES 39

typedef struct {
	unsigned char key;
	char value[64];

} fstype;

typedef struct {
	unsigned char status;
	unsigned char chsBegin[3];
	unsigned char fsID;
	unsigned char chsEnd[3];
	unsigned int firstLBA;
	unsigned int sectorCount;
} partinfo;

void detectMBRFS(FILE *fptr, int *sectorSize);
fstype binarySearchFSTypes(short low, short high, unsigned char partitionID);

#endif