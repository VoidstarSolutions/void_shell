#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "void_command.h"
#include "void_shell.h"

int8_t shell_get_char( void ) { return getchar(); }
//NOLINTNEXTLINE
int _putchar( char character ) { return putchar( character ); }

void shell_output( const char *data, size_t length )
{
	for ( size_t i = 0; i != length; ++i )
	{
		_putchar( data[i] );
	}
}

static bool run_shell = true;

static void exit_command( void ) { run_shell = false; }

static const struct vc_description exit_command_description = {
    .command_string = "exit",
    .command        = exit_command,
    .help_string    = "Exit the app",
};

int main()
{
	system( "/bin/stty raw" );
	vs_init();
	vs_configure( vs_shell_handles[0], &shell_get_char, &shell_output, true );
	vc_init( vs_shell_handles[0] );
	vc_register( &exit_command_description );
	while ( run_shell )
	{
		vs_run( vs_shell_handles[0] );
	}
	system( "/bin/stty cooked" );
	return 0;
}
