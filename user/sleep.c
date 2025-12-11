#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char* argv[]) {
    if (argc > 1) {
        pause(atoi(argv[1]));
    }
    else {
        printf("Error: Provide an argument!\n");
    }

    exit(0);
}