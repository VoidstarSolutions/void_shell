#include <stdint.h>

#include "SEGGER_RTT.h"

#include "void_shell.h"

int8_t shell_get_char(void)
{
    return SEGGER_RTT_GetKey();
}

//NOLINTNEXTLINE
int _putchar(char character)
{
    return SEGGER_RTT_PutChar( 0, character);
}

int main()
{
     vs_init(vs_static_shell, &shell_get_char);
    while (true)
    {
         vs_run(vs_static_shell);
    }
}
