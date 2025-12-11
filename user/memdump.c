#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("I am too lazy to add example inputs. Provide the args :/\n");
        exit(0);
    }

    char* fmt = argv[1];
    char buf[1024];

    int n = read(0, buf, sizeof(buf));

    if (n <= 0) {
        exit(0);
    }

    char* data = buf;

    while (*fmt) {
        switch (*fmt) {
            case 'i': {
                int val = *(int*)data;
                printf("%d\n", val);
                data += 4;
                break;
            }
            case 'p': {
                uint64 val = *(uint64*)data;
                printf("%ld\n", val);
                data += 8;
                break;
            }
            case 'h': {
                short val = *(short*)data;
                printf("%d\n", val);
                data += 2;
                break;
            }
            case 'c': {
                char val = *data;
                printf("%c\n", val);
                data += 1;
                break;
            }
            case 's': {
                char* str_ptr = *(char**)data;
                printf("%s\n", str_ptr);
                data += 8;
                break;
            }
            case 'S': {
                printf("%s\n", data);
                exit(0);
            }
        }
        fmt++;
    }
    exit(0);
}