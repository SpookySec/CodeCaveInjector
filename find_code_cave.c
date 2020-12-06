/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description: finds a code cave in any binary and returns the address. 
 *
 *        Version:  1.0
 *        Created:  07/27/2020 11:54:56 PM
 *       Compiler:  gcc
 *
 *         Author:  spooky_sec
 *
 * =====================================================================================
 */
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>
#include <errno.h>

typedef struct {
    int size;
    int start;
    int end;
} code_cave_t;

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage %s <binary>\n", argv[0]);
        return 1;
    }

	int current_cave_size = 0, byte = 0, currentByte = 0;
	long int file_size = 0;
	char * fileMem = NULL;
	struct stat sb;

	char * bin_path = argv[1];

    code_cave_t cc;
    cc.size = 0;

    int fd = open(bin_path, O_RDONLY, S_IRUSR | S_IWUSR);

    lseek(fd, 0, SEEK_SET);

    if (fstat(fd, &sb) == -1) {
        printf("Error with fstat\n");
        return -1;
    }

    file_size = sb.st_size;

    printf("[%p] File Size: %ld\n", &file_size, file_size);
    fileMem = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

    for (byte = 0; byte < file_size; byte++) {
        currentByte = fileMem[byte];

		if ((int)currentByte == 0x00) {
			current_cave_size++;
		} else {
			if (current_cave_size > 0) {
				if (current_cave_size > cc.size) {
					cc.size = current_cave_size;
					cc.end = byte;
					cc.start = byte - current_cave_size;
				}
				current_cave_size = 0;
			}
		}
	}

    printf("[%p] BYTES : %d\n", &cc.size, cc.size);
    printf("[%p] START : 0x%X\n", &cc.start, cc.start);
    printf("[%p] END   : 0x%X\n", &cc.end, cc.end);

    return 0;
}

