// Purpose: provide (a modified) chrome access to pagemap.
// Compile with gcc pagemap_access.c -o pagemap_access, run as root.

#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#define PAGE_SIZE 4096
#define PIPE_PATH_REQ "/tmp/pagemap_access_req"
#define PIPE_PATH_OUT "/tmp/pagemap_access_out"

static volatile int keepRunning = 1;

typedef struct {
    int pid;
    uintptr_t address;
} request;

void intHandler(int dummy) {
    keepRunning = 0;
}

uint64_t virt_to_physical_page_number(pid_t pid, uint64_t vaddr) {
    char path[100] = {0};
    snprintf(path, sizeof(path), "/proc/%d/pagemap", pid);
    int pagemap_fd = open(path, O_RDONLY);

    unsigned long paddr = -1;
    unsigned long index = (vaddr / PAGE_SIZE) * sizeof(paddr);
    if (pread (pagemap_fd, &paddr, sizeof(paddr), index) != sizeof(paddr)) {
        return -1;
    }
    paddr &= 0x7fffffffffffff;
    return paddr;
}

size_t read_all(int fd, char *buf, size_t count) {
    size_t so_far = 0;
    while (so_far < count) {
        size_t current = read(fd, buf + so_far, count - so_far);
        if (current == -1)
            return current;
        so_far += current;
    }
}

int main(int argc, char *argv[]) {
    signal(SIGINT, intHandler);
    umask(0);
    int fd1 = mkfifo(PIPE_PATH_REQ, 0777);
    int fd2 = mkfifo(PIPE_PATH_OUT, 0777);
    int fd_r = open(PIPE_PATH_REQ, O_RDONLY);
    int fd_w = open(PIPE_PATH_OUT, O_WRONLY);
 
    while (keepRunning) {
        request req = {0};
        read_all(fd_r, &req, sizeof(req));
 
        uint64_t result = virt_to_physical_page_number(req.pid, req.address);
        write(fd_w, &result, sizeof(result));
    }

    close(fd_r);
    close(fd_w);
    close(fd1);
    close(fd2);
 
    unlink(PIPE_PATH_REQ);
    unlink(PIPE_PATH_OUT);
    return 0;
}