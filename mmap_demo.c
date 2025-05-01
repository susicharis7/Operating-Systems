#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

int main() {
    printf("\n|=== Project 2 [Sanin & Haris] DEMO ===|");

    // print initial memory usage using ps
    printf("\n[1] Initial memory usage (before mmap):\n");
    system("ps -o pid,vsz,rss,comm -p $(pidof mmap_demo)");

    // allocate 4KB (1 page) using mmap
    void *addr = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    // if it fails, return error
    if (addr == MAP_FAILED) {
        perror("mmap failed");
        return EXIT_FAILURE;
    }
    
    printf("\n[2] Memory usage after mmap:\n");
    system("ps -o pid,vsz,rss,comm -p $(pidof mmap_demo)");

    // write to the allocated memory
    strcpy((char *)addr, "Memory test successful!");

    printf("\n[3] Memory usage after writing to mmap'd memory:\n");
    system("ps -o pid,vsz,rss,comm -p $(pidof mmap_demo)");

    // optional - print what was written
    printf("\n[4] Content written to memory: %s\n", (char *)addr);

    // unmap the memory
    if (munmap(addr, 4096) == -1) {
        perror("munmap failed");
        return EXIT_FAILURE;
    }

    printf("\n[5] Memory unmapped successfully.\n");

    // keep it alive for 30sec so we can test it
    sleep(30);

    return 0;
}
