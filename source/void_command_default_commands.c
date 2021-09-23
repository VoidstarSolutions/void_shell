#include "void_command_default_commands.h"

#include "../third_party/printf/printf.h"

#include "void_command.h"
#include "void_shell.h"

static void help_command( int arg_c, char **arg_v )
{
	(void) ( arg_c );
	(void) ( arg_v );
	for ( uint16_t i = 1; i < vc->registered_command_count; ++i )
	{
		const struct vc_description *desc = vc->registered_commands[i];
		printf( "%s\r\n  ", desc->command_string );
		printf( "%s\r\n", desc->help_string );
	}
}

const struct vc_description default_help_command_description = {
    .command_string = "help",
    .command        = help_command,
    .help_string    = "The following are the currently registered commands:",
};

static void clear_command( int argc, char **argv )
{
	(void) ( argc );
	(void) ( argv );
	vs_clear_console( vs_default_shell );
}

const struct vc_description default_clear_command_description = {
    .command_string = "clear",
    .command        = clear_command,
    .help_string    = "Clear the terminal",
};
