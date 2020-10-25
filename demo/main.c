#include <stdint.h>
#include <stdio.h>

#include "void_shell.h"

int8_t void_shell_get_char( void )
{
    return getchar();
}

int _putchar(char character) { return putchar(character); }

int main(int argc, char *argv[])
{
    void_shell_init();
    while (true)
    {
        void_shell_run();
    }
}