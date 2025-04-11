#include "fsinfo.h"

fstype FILESYSTEMS[NUM_FS_TYPES] = {
	{.key = 0x00, .value = "No Partition/ISO9660"},
	{.key = 0x01, .value = "FAT12"},
	{.key = 0x04, .value = "FAT16"},
	{.key = 0x05, .value = "DOS extended partiton"},
	{.key = 0x06, .value = "FAT16 (DOS 3.31+ Large FS)"},
	{.key = 0x07, .value = "NTFS/HPFS/exFAT"},
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

void detectMBRFS(FILE *fptr, int *sectorSize) {
	partinfo partitionsInfo[4];
	
	for (int i = 0; i < 4; i++) {
		fseek(fptr, (0x1BE + (0x10 * i)), SEEK_SET);
		fread((partitionsInfo+i), sizeof(unsigned char), 16, fptr);
	}

	int isIso = 1;

	printf("Analyzing MBR...\n\n");


	for (int i = 0; i < 4; i++) {
		fstype partType = binarySearchFSTypes(0, (NUM_FS_TYPES - 1), partitionsInfo[i].fsID);
		if (strcmp(partType.value, "No Partition/ISO9660") != 0) {
			isIso = 0;
		}
		printf("Partition %d type: %s\n", (i + 1), partType.value);
		printf("Status: ");
		switch (partitionsInfo[i].status) {
			case 0:
				printf("Inactive/Non-bootable");
				break;
			default:
				if (partitionsInfo[i].status & 0x80) {
					printf("Active/Bootable");
				}
				else {
					printf("Invalid");
				}
		}
		printf("\nSize: %.2f MB\n", ((float)(512 * partitionsInfo[i].sectorCount) / 1048576.0));
		printf("Start sector: %d\n\n", partitionsInfo[i].firstLBA);
		
	}

	if (isIso) {
		printf("Detected ISO9660 file system, defaulting to 2048 sector size\n");
		if (!(*sectorSize)) {
			*sectorSize = 2048;
		}
	}
	else {
		printf("Detected MBR disk image, defaulting to 512 sector size\n");
		if (!(*sectorSize)) {
			*sectorSize = 512;
		}
	}

	fseek(fptr, 0, SEEK_SET);
	return;
}

fstype binarySearchFSTypes(short low, short high, unsigned char partitionID) {
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
		return FILESYSTEMS[mid];
	}
	fstype unknown = {.key=-1, .value="Unknown filesystem or no partition"};
	return unknown;
}