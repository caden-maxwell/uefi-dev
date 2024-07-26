#include <uefi.h>

/**
 * Classic Hello World example
 */
int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("Hello World!\n\n");

    printf("Press any key to exit...\n");
    getchar();

    return 0;
}
