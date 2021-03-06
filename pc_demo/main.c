#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "void_command.h"
#include "void_shell.h"

int8_t void_shell_get_char( void ) { return getchar(); }
//NOLINTNEXTLINE
int _putchar( char character ) { return putchar( character ); }

static bool run_shell = true;

static void exit_command( void ) { run_shell = false; }

static const struct void_command_description exit_command_description = { .command_string = "exit",
                                                                          .command = exit_command,
                                                                          .help_string =
                                                                              "Exit the app",
                                                                          .sub_commands = NULL };

int main()
{
	system( "/bin/stty raw" );
	void_shell_init();
	vc_register( &exit_command_description );
	while ( run_shell )
	{
		void_shell_run();
	}
	system( "/bin/stty cooked" );
	return 0;
}
