#include "fsinfo.h"

void detectMBRFS(FILE *fptr, int *sectorSize) {
	unsigned char partitionID;
	unsigned char partitionIDs[4];
	
	for (int i = 0; i < 4; i++) {
		fseek(fptr, (0x1C2 + (0x10 * i)), SEEK_SET);
		partitionID = fread((partitionIDs+i), sizeof(unsigned char), 1, fptr);
	}

	int isIso = 1;

	printf("Analyzing MBR...\n");


	for (int i = 0; i < 4; i++) {
		char* partType = binarySearchFSTypes(0, (NUM_FS_TYPES - 1), partitionIDs[i]);
		if (strcmp(partType, "No Partition/ISO9660") != 0) {
			isIso = 0;
		}
		printf("Partition %d type: %s\n", i, partType);
	}

	if (isIso) {
		printf("Detected ISO9660 file system, defaulting to 2048 sector size\n");
		if (!(*sectorSize)) {
			*sectorSize = 2048;
		}
	}
	else {
		printf("Detected disk image, defaulting to 512 sector size");
		if (!(*sectorSize)) {
			*sectorSize = 512;
		}
	}

	fseek(fptr, 0, SEEK_SET);
	return;
}

char* binarySearchFSTypes(short low, short high, unsigned char partitionID) {
	short mid = (low + ((high - low) / 2));
	unsigned char key = FILESYSTEMS[mid].key;
	if (high > low) {
		if (key > partitionID) {
			return binarySearchFSTypes(low, mid, partitionID);
		}
		else if (key < partitionID) {
			return binarySearchFSTypes(mid, high, partitionID);
		}
	}
	if (key == partitionID) {
		return FILESYSTEMS[mid].value;
	}
	return "Unknown filesystem or no partition";
}