/*
 * =====================================================================================
 *
 *       Filename:  findcave.h
 *
 *    Description:  a header file to find the code cave 
 *
 *        Version:  1.0
 *        Created:  11/27/2020 09:59:22 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SPOOKY (@spooky_sec), https://discord.gg/UTVBvJs
 *   Organization:  
 *
 * =====================================================================================
 */

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
    long file_size;
    int size;
    int start;
    int end;
    int bytes_written;
} code_cave_t;

int find_code_cave(char *bin_path, code_cave_t *cc) {

    int fd = open(bin_path, O_RDONLY, S_IRUSR | S_IWUSR);

    if (fd == -1) {
        printf("File: %s not found!\n", bin_path);
        return -1;
    }

	int current_cave_size = 0, byte = 0, currentByte = 0;
	cc->file_size = 0;
    cc->size = 0;
    cc->bytes_written = 0;

	char *fileMem = NULL;
	struct stat sb;

    if (fstat(fd, &sb) == -1) {
        printf("Error with fstat\n");
        return -1;
    }

    cc->file_size = sb.st_size;

    fileMem = (char *)mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

    for (byte = 0; byte < cc->file_size; byte++) {
        currentByte = fileMem[byte];

		if ((int)currentByte == 0x00) {
			current_cave_size++;
		} else {
			if (current_cave_size > 0) {
				if (current_cave_size > cc->size) {
					cc->size = current_cave_size;
					cc->end = byte;
					cc->start = byte - current_cave_size;
				}
				current_cave_size = 0;
			}
		}
	}

    return 0;
}

