#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "void_command.h"
#include "void_shell.h"
#include "void_shell_utilities.h"

int8_t shell_get_char( void ) { return getchar(); }

//NOLINTNEXTLINE
void _putchar( char character ) { putchar( character ); }

void shell_output( const char *data, size_t length )
{
	for ( size_t i = 0; i != length; ++i )
	{
		_putchar( data[i] );
	}
}

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

void vc_print_greeting( const_vs_handle shell )
{
	vs_text_color( shell, COLOR_GREEN );
	vs_more_bold( shell );
	vs_more_bold( shell );
	printf( "  _    _  _____  _____ ______       _______ _     _ _______               \r\n" );
	printf( "   \\  /  |     |   |   |     \\      |______ |_____| |______ |      |      \r\n" );
	printf( "    \\/   |_____| __|__ |_____/      ______| |     | |______ |_____ |_____ \r\n\r\n" );

	vs_reset_format( shell );
}

int main()
{
	system( "/bin/stty raw" );
	vs_init( vs_default_shell );
	vs_configure( vs_default_shell, &shell_get_char, &shell_output, true );
	vc_init();
	vc_activate( vs_default_shell );
	vc_register( &exit_command_description );
	while ( run_shell )
	{
		vs_run( vs_default_shell );
	}
	system( "/bin/stty cooked" );
	return 0;
}
