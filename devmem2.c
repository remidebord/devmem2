/*
 * devmem2.c: Simple program to read/write from/to any location in memory.
 *
 *  Copyright (C) 2000, Jan-Derk Bakker (jdb@lartmaker.nl)
 *
 *
 * This software has been developed for the LART computing board
 * (http://www.lart.tudelft.nl/). The development has been sponsored by
 * the Mobile MultiMedia Communications (http://www.mmc.tudelft.nl/)
 * and Ubiquitous Communications (http://www.ubicom.tudelft.nl/)
 * projects.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <getopt.h>

#define FATAL do { fprintf(stderr, "Error at line %d, file %s (%d) [%s]\n", \
  __LINE__, __FILE__, errno, strerror(errno)); exit(1); } while(0)
 
#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)

char width[] = {
	['b'] = (8 * sizeof(char)),
	['h'] = (8 * sizeof(short)),
	['w'] = (8 * sizeof(int)),
	['l'] = (8 * sizeof(long)),
};

void print_hex(char *buffer, int length)
{
	char hex[64] = {0};
	char ascii[64] = {0};
	int addr = 0;

	for(int i = 0; i < length; i++) {
		if (((i % 16) == 0)) {
			if(i != 0) {
				printf("%08x: %s %s\n", addr, hex, ascii);
				memset(hex, 0x00, sizeof(hex));
				memset(ascii, 0x00, sizeof(ascii));
			}
			addr = i;
		}

		sprintf(hex + strlen(hex), "%02x%c", buffer[i], (i % 2) ? ' ' : '\0');
		sprintf(ascii + strlen(ascii), "%c", ((buffer[i] >= 0x20) && (buffer[i] <= 0x7E)) ? buffer[i] : '.');
	}

	if(hex[0] != 0x00)
		printf("%08x: %s %s\n", addr, hex, ascii);
}

void usage(char *name)
{
	printf("Usage:\t%s [ options ] { address } [ type / length [ data ] ]\n", name);
	printf("\taddress       : memory address to act upon\n");
	printf("\ttype / length : access operation type : [b]yte, [h]alfword, [w]ord, or access length\n");
	printf("\tdata          : data to be written\n");
	printf("Options:\n");
	printf("\t--file, -f      : file to map (/dev/mem by default)\n");
	printf("\t--read-only, -r : open file in read-only mode\n");
	printf("\t--help, -h      : display this help and exit\n");
}

int main(int argc, char **argv) {
    int fd;
    void *map_base, *virt_addr; 
	unsigned long read_result, writeval;
	off_t target;

	char *filename = "/dev/mem";
	int access_type = 'w';
	int access_length = 0;
	int oflags = O_RDWR | O_SYNC;
	int mflags = PROT_READ | PROT_WRITE;

	static struct option long_options[] =
	{
		{"file"     , required_argument, 0, 'f'},
		{"read-only", no_argument      , 0, 'r'},
		{"help"     , no_argument      , 0, 'h'},
		{0, 0, 0, 0}
	};

	int option_index = 0;
	int c;

	while ((c = getopt_long(argc, argv, "c:rh", long_options, &option_index)) != -1) {
		switch(c) {
			case 'f':
				filename = optarg;
				break;
			case 'r':
				oflags = O_RDONLY;
				mflags = PROT_READ;
				break;
			case 'h':
				usage(argv[0]);
				exit(1);
				break;
		}
	}

	if (optind)
		argc = argc - (optind - 1);

	if (argc < 2) {
		usage(argv[0]);
		exit(1);
	}

	target = strtoul(argv[optind++], 0, 0);

	if (argc > 2) {
		access_type = tolower(argv[optind][0]);
		if((access_type >= '0') && (access_type <= '9')) {
			access_length = strtoul(argv[optind], 0, 0);
			access_type = 'n';
		}
		optind++;
	}

    if ((fd = open(filename, oflags)) == -1) FATAL;
    printf("%s opened.\n", filename); 
    fflush(stdout);

    /* Map one page */
    map_base = mmap(0, MAP_SIZE, mflags, MAP_SHARED, fd, target & ~MAP_MASK);
    if (map_base == (void *) -1) FATAL;
    printf("Memory mapped at address %p.\n", map_base); 
    fflush(stdout);
    
    virt_addr = map_base + (target & MAP_MASK);
    
	/* Write */
	if (argc > 3) {
		if (oflags == O_RDONLY) {
			printf("cannot write, file is in read-only mode!\n");
			goto unmap_file;
		}

		writeval = strtoul(argv[optind], 0, 0);
		switch(access_type) {
			case 'b':
				*((unsigned char *) virt_addr) = writeval;
				break;
			case 'h':
				*((unsigned short *) virt_addr) = writeval;
				break;
			case 'w':
				*((unsigned int *) virt_addr) = writeval;
				break;
			case 'n':
				memset(virt_addr, writeval, access_length);
				break;
		}
	/* Read */
	} else {
		switch (access_type) {
			case 'b':
				read_result = *((unsigned char *) virt_addr);
				break;
			case 'h':
				read_result = *((unsigned short *) virt_addr);
				break;
			case 'w':
				read_result = *((unsigned int *) virt_addr);
				break;
			case 'n':
				print_hex(virt_addr, access_length);
				break;
			default:
				fprintf(stderr, "Illegal data type '%c'.\n", access_type);
				exit(2);
		}

		if (access_type != 'n')
			printf("0x%0*llx\n", (width[access_type] >> 2), (unsigned long long)read_result);
	}

    fflush(stdout);

unmap_file:
	if (munmap(map_base, MAP_SIZE) == -1) FATAL;
    close(fd);
    return 0;
}

