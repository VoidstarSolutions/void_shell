#include <stdint.h>

#include "SEGGER_RTT.h"

#include "void_shell.h"

int8_t void_shell_get_char(void)
{
    return SEGGER_RTT_GetKey();
}

int _putchar(char character)
{
    return SEGGER_RTT_PutChar( 0, character);
}

int main()
{
    void_shell_init();
    while (true)
    {
        void_shell_run();
    }
}
