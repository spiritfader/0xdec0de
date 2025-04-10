#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fsinfo.h"


int main(int argc, char *argv[]) {
	int sectorSize = 0;
	int lba = 0;
	int address = 0;

	if (strcmp(argv[1], "--help") == 0) {
		printf("0xdec0de\n");
		printf("--help: shows this menu.\n");
		printf("--lbasize $SIZE: specify the LBA size to divide the output into, in bytes.\n");
		printf("--sector $SECTOR: specify the sector to begin the output.\n");
		printf("--address $ADDRESS: specify the address to begin the output, as a hexadecimal offset from 0, rounded down to the nearest 16-byte line.\n");
		printf("If both --sector and --address are present, output will begin from the point specified by --sector.\n");
		return 0;
	}

	if (argc < 2) {
		printf("Insufficient number of arguments. Please make sure you provide one filename as the first argument for the program.");
		return 1;
	}

	for (int i = 2; i < argc; i++) {
		if (strcmp(argv[i], "--lbasize") == 0) {
			sectorSize = atoi(argv[++i]);
		}
		else if (strcmp(argv[i], "--sector") == 0) {
			lba = atoi(argv[++i]);
		}
		else if (strcmp(argv[i], "--address") == 0) {
			address = (int)strtol(argv[++i], NULL, 16);
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
			//printf("\nSector: %d\n________________________________________________________________________________\n", lba);
			printf("\nSector: %d\n_____________0__1__2__3__4__5__6__7___8__9__A__B__C__D__E__F____________________\n", lba);
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

