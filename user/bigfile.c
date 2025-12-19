#include <kernel/types.h>
#include <kernel/stat.h>
#include <kernel/fs.h>
#include <user/user.h>
#include <kernel/fcntl.h>

#define BUFFER_SIZE 1024

int main() {
    int fd, i;
    char buf[BUFFER_SIZE];
    int blocks = 65803;
    
    fd = open("bigfile", O_CREATE | O_WRONLY);

    if (fd < 0) {
        printf("Could not open file\n");
        exit(1);
    }

    memset(buf, 'A', sizeof(buf));

    for (i = 0; i < blocks; i++) {
        if (i % 100 == 0) printf("%d\n", i);
        int n = write(fd, buf, BUFFER_SIZE);
        if (n != BUFFER_SIZE) {
            printf("bigfile: write failed at block %d\n", i);
            exit(1);
        }
    }

    close(fd);

    // Check the size of the resulting file
    struct stat st;
    if(stat("bigfile", &st) < 0){
        printf("bigfile: stat failed\n");
        exit(1);
    }

    printf("bigfile: success! file size is %ld bytes\n", st.size);

    exit(0);
}