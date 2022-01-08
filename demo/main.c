#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "void_command.h"
#include "void_shell.h"
#include "void_shell_utilities.h"

int8_t shell_get_char( void ) { return getchar(); }

void shell_put_char( char out_char ) { putchar( out_char ); }

static bool run_shell = true;
static void exit_command( int argc, char **argv )
{
    (void) ( argc );
    (void) ( argv );
    run_shell = false;
}

static const struct vc_description exit_command_description = {
    .command_string = "exit",
    .command        = exit_command,
    .help_string    = "Exit the app",
};

void vc_print_greeting()
{
    vs_text_color( COLOR_GREEN );
    vs_more_bold();
    vs_more_bold();
    printf( "  _    _  _____  _____ ______       _______ _     _ _______               \r\n" );
    printf( "   \\  /  |     |   |   |     \\      |______ |_____| |______ |      |      \r\n" );
    printf( "    \\/   |_____| __|__ |_____/      ______| |     | |______ |_____ |_____ \r\n\r\n" );

    vs_reset_format();
}

int main()
{
    system( "/bin/stty raw" );
    vs_configure( &shell_get_char, &shell_put_char );
    vc_init();
    vc_register( &exit_command_description );
    while ( run_shell )
    {
        vs_run();
    }
	vs_clear_console();
    system( "/bin/stty cooked" );
    return 0;
}
