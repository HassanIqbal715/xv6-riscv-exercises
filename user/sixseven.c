#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        printf("Error! Path argument not provided.\n");
        exit(0);
    }
    
    // Passing O_RDONLY
    int fd = open(argv[1], O_RDONLY);

    if (fd < 0) {
        printf("Error: Could not open file!\n");
        exit(0);
    }

    // Now reading from the path
    int bufferSize = 500;
    char* buffer = malloc(sizeof(char) * bufferSize);
    int sizeRead = read(fd, buffer, bufferSize);

    int flag = 0;
    int temp = 0;
    int countRead = 0;

    for (int i = 0; i < sizeRead; i++) {
        if (flag == 1) {
            char tempBuf = buffer[i];
            if (tempBuf == '0') {
                countRead++;
                temp *= 10;
            }
            else {
                if (atoi(&tempBuf) != 0) {
                    temp *= 10;
                    temp += atoi(&tempBuf);
                    countRead++;
                }
            }
        }

        if (strchr("-\\./,", buffer[i])) {
            if (buffer[i] == '\\') {
                if (i == sizeRead - 1)
                    continue;
                if (!strchr("rnt", buffer[i + 1]))
                    continue;
            }

            if (flag == 1 && countRead > 0 && (temp % 7 == 0 || temp % 6 == 0)) {
                printf("%d\n", temp);
                temp = 0;
                countRead = 0;
            }

            flag = !flag;
        }
    }

    free(buffer);

    if (fd >=0) {
        close(fd);
        exit(0);
    } 

    exit(0);
}