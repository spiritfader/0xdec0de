#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	unsigned char key;
	char value[64];
} fstype;

#define NUM_FS_TYPES 39

fstype FILESYSTEMS[NUM_FS_TYPES] = {
	{.key = 0x00, .value = "No Partition/ISO9660"},
	{.key = 0x01, .value = "FAT12"},
	{.key = 0x04, .value = "FAT16"},
	{.key = 0x05, .value = "DOS extended partiton"},
	{.key = 0x06, .value = "FAT16 (DOS 3.31+ Large FS)"},
	{.key = 0x07, .value = "NTFS/HPFS"},
	{.key = 0x0A, .value = "Apple HPFS"},
	{.key = 0x0B, .value = "W95 FAT32"},
	{.key = 0x0C, .value = "W95 FAT32 (LBA mode)"},
	{.key = 0x0E, .value = "VFAT 16 (LBA mode)"},
	{.key = 0x0F, .value = "DOS extended partiton (LBA mode)"},
	{.key = 0x12, .value = "FAT12 (Hidden)"},
	{.key = 0x14, .value = "FAT16 (Hidden)"},
	{.key = 0x16, .value = "FAT16 (Hidden)"},
	{.key = 0x16, .value = "HPFS (Hidden)"},
	{.key = 0x17, .value = "NTFS (Hidden)"},
	{.key = 0x1B, .value = "W95 FAT32 (Hidden)"},
	{.key = 0x1C, .value = "W95 FAT32 (Hidden) (LBA mode)"},
	{.key = 0x1E, .value = "VFAT/Unknown (Hidden) (LBA mode)"},
	{.key = 0x81, .value = "Linux/Unknown"},
	{.key = 0x82, .value = "Linux Swap/Solaris"},
	{.key = 0x83, .value = "Linux EXT2/3/4"},
	{.key = 0x85, .value = "Linux EXT/Unspecified"},
	{.key = 0x86, .value = "Windows NT FAT16 (Striped)"},
	{.key = 0x87, .value = "NTFS (Striped)/HPFS (Mirrored)"},
	{.key = 0xB6, .value = "Windows NT FAT16 (Mirrored Master)"},
	{.key = 0xB7, .value = "NTFS (Mirrored Master)/BSDI"},
	{.key = 0xC6, .value = "Windows NT FAT16 (Mirrored Slave/Corrupt)"},
	{.key = 0xC7, .value = "NTFS (Mirrored Slave/Corrupt)"},
	{.key = 0xCB, .value = "DR-DOS Secured FAT32"},
	{.key = 0xCC, .value = "DR-DOS Secured FAT32 (LBA mode)"},
	{.key = 0xCD, .value = "DR-DOS Secured FAT16 (LBA mode)"},
	{.key = 0xD0, .value = "Multi-user DOS secured FAT12"},
	{.key = 0xD1, .value = "Multi-user DOS secured FAT12"},
	{.key = 0xD4, .value = "Multi-user DOS secured FAT16"},
	{.key = 0xD6, .value = "Multi-user DOS secured FAT16 (LBA mode)"},
	{.key = 0xEB, .value = "BFS"},
	{.key = 0xF2, .value = "DOS secondary partition"},
	{.key = 0xFB, .value = "VMWare partition"},
};

void detectMBRFS(FILE *fptr, int *sectorSize);
char* binarySearchFSTypes(short low, short high, unsigned char partitionID);

int main(int argc, char *argv[]) {
	int sectorSize = 0;
	int lba = 0;
	int address = 0;
	
	if (argc < 2) {
		printf("Insufficient number of arguments. Please make sure you provide one filename as the first argument for the program.");
		return 1;
	}
	/*
	else if (argc == 2) {
		sectorSize = 2048;
	}
	else {
		sectorSize = atoi(argv[2]);
	} */

	for (int i = 2; i < argc; i++) {
		
		if (strcmp(argv[i], "--lbasize") == 0) {
			sectorSize = atoi(argv[++i]);
		}
		else if (strcmp(argv[i], "--sector") == 0) {
			lba = atoi(argv[++i]);
		}
		else if (strcmp(argv[i], "--address") == 0) {
			address = atoi(argv[++i]);
		}
	}

	if (lba) {
		address = (lba * sectorSize);
	}

	address -= (address % 16);

	FILE *fptr = fopen(argv[1], "rb");
	if (fptr == NULL) {
		printf("Couldn't open file - please make sure the filename is valid");
		return 1;
	}

	detectMBRFS(fptr, &sectorSize);

	if (address) {
		fseek(fptr, address, SEEK_SET);
	}
	

	unsigned char data[16];
	size_t n;
	int secIdent = address;
	while ((n = fread(data, sizeof(unsigned char), 16, fptr)) > 0) {
		
		int n;
		lba = (address / sectorSize);

		if (address == secIdent) {
			printf("\nSector: %d\n________________________________________________________________________________\n", lba);
			secIdent = (secIdent + sectorSize);
		}
		
		printf("%.10X  %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X  %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X  |", address, \
			data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], \
			data[9], data[10], data[11], data[12], data[13], data[14], data[15]);
		address = (address + 16);

		for (int i = 0; i < 16; i++) {
			char cur = data[i];
			if (cur < 32 || cur > 126) {
				printf(".");
			}
			else {
				printf("%c", data[i]);
			}
		}
		printf("|\n");
	}	
	fclose(fptr);
	return 0;
}

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
		printf("Detected ISO9660 file system - recommend sector size 2048\n");
		if (!(*sectorSize)) {
			*sectorSize = 2048;
		}
	}
	else {
		printf("Detected disk image - recommend sector size 512");
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