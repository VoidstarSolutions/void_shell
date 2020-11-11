#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "void_shell.h"

int8_t void_shell_get_char(void)
{
    return getchar();
}

int _putchar(char character)
{
    return putchar(character);
}

int main()
{
     system ("/bin/stty raw");
    void_shell_init();
    while (true)
    {
        void_shell_run();
    }
    return -1;
}
