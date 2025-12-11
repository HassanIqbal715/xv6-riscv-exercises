#include "kernel/types.h"
#include "user/user.h"

int 
main(void)
{
    printf("Hello world!\n");
    int result = mycall(5, 7);
    printf("Result of 5 + 7 = %d\n", result);
    exit(0);
}