/*==============================================================================
**
** Copyright (c) 2021 Voidstar Solutions
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all
** copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
** SOFTWARE.
**
==============================================================================*/

/**
 * \file void_command.c
 *
 * \date Created: 06/04/20
 * \author Zachary Heylmun
 *
 */

#include "void_command.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include <printf.h>

#include "void_shell.h"
#include "void_shell_utilities.h"

#ifndef VC_MAX_COMMANDS
#define VC_MAX_COMMANDS ( (uint16_t) 32 )
#endif // VC_MAX_COMMANDS

#ifndef VC_MAX_COMMAND_LEN
#define VC_MAX_COMMAND_LEN ( (uint16_t) 32 )
#endif // VC_MAX_COMMAND_LEN

struct vc_data
{
	vs_handle                    shell;
	uint16_t                     registered_command_count;
	struct vc_description const *registered_commands[VC_MAX_COMMANDS];
	struct vc_description const *active_modal_command;
};

static struct vc_data void_command_instance;

static void help_command( void )
{
	const struct vc_data *command = &void_command_instance;
	for ( uint16_t i = 1; i < command->registered_command_count; ++i )
	{
		const struct vc_description *desc = command->registered_commands[i];
		printf( "%s\r\n  ", desc->command_string );
		printf( "%s\r\n", desc->help_string );
	}
}

static const struct vc_description help_command_description = {
    .command_string = "help",
    .command        = help_command,
    .help_string    = "The following are the currently registered commands:",
};

static void clear_command( void ) { vs_clear_console( vs_shell_handles[0] ); }

static const struct vc_description clear_command_description = {
    .command_string = "clear",
    .command        = clear_command,
    .help_string    = "Clear the terminal",
};

void vc_init( vs_handle shell )
{
	struct vc_data *command           = &void_command_instance;
	command->shell                    = shell;
	command->registered_command_count = 0;
	command->active_modal_command     = NULL;
	vc_register( &help_command_description );
	vc_register( &clear_command_description );
	vs_text_color( shell, COLOR_GREEN );

	vs_more_bold( shell );
	vs_more_bold( shell );
	printf( "  _    _  _____  _____ ______       _______ _     _ _______               \r\n" );
	printf( "   \\  /  |     |   |   |     \\      |______ |_____| |______ |      |      \r\n" );
	printf( "    \\/   |_____| __|__ |_____/      ______| |     | |______ |_____ |_____ \r\n\r\n" );

	vs_reset_format( shell );
	vc_print_context();
}

bool vc_register( const struct vc_description *description )
{
	assert( strnlen( description->command_string, VC_MAX_COMMAND_LEN + 1 ) <= VC_MAX_COMMAND_LEN );
	struct vc_data *command = &void_command_instance;
	if ( command->registered_command_count == VC_MAX_COMMANDS )
	{
		printf( "Max Commands reached.  Please increasesID_COMMAND_MAX_COMMANDS\r\n" );
		return false;
	}
	command->registered_commands[command->registered_command_count++] = description;
	return true;
}

size_t vc_complete_command( char *in_out_string, size_t max_len )
{
	(void) ( max_len );
	const struct vc_data *command     = &void_command_instance;
	size_t                input_len   = strnlen( in_out_string, max_len );
	uint16_t              match_index = command->registered_command_count;
	for ( uint16_t command_index = 0; command_index != command->registered_command_count;
	      ++command_index )
	{
		struct vc_description const *desc = command->registered_commands[command_index];
		if ( input_len <= strnlen( desc->command_string, VC_MAX_COMMAND_LEN ) )
		{
			bool match = true;
			for ( uint16_t char_index = 0; char_index != input_len; ++char_index )
			{
				if ( in_out_string[char_index] != desc->command_string[char_index] )
				{
					match = false;
				}
			}
			if ( match )
			{
				if ( match_index == command->registered_command_count )
				{
					match_index = command_index;
				}
				else
				{
					return 0;
				}
			}
		}
	}
	if ( match_index != command->registered_command_count )
	{
		struct vc_description const *match   = command->registered_commands[match_index];
		size_t                       new_len = strnlen( match->command_string, VC_MAX_COMMAND_LEN );
		for ( uint16_t char_index = 0; char_index != new_len; ++char_index )
		{
			in_out_string[char_index] = match->command_string[char_index];
		}
		return new_len;
	}
	return 0;
}

void vc_handle_command( const char *command_string )
{
	bool                  command_found = false;
	const struct vc_data *command       = &void_command_instance;
	for ( uint16_t command_index = 0; command_index != command->registered_command_count;
	      ++command_index )
	{
		if ( strcmp( command->registered_commands[command_index]->command_string,
		             command_string ) == 0 )
		{
			command->registered_commands[command_index]->command();
			command_found = true;
			break;
		}
	}
	if ( !command_found )
	{
		printf( "Command not recognized.\r\n\tTry \"help\"\r\n" );
	}
	vc_print_context();
}

void vc_print_context( void )
{
	struct vc_data *command = &void_command_instance;
	vs_text_color( command->shell, COLOR_YELLOW );
	if ( command->active_modal_command )
	{
		printf( "%s", command->active_modal_command->command_string );
	}
	printf( "$>" );
	vs_reset_format( command->shell );
}
