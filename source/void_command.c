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

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include <printf.h>

#include "void_shell.h"
#include "void_shell_utilities.h"

#ifndef VOID_COMMAND_MAX_COMMANDS
#define VOID_COMMAND_MAX_COMMANDS ( (uint16_t) 32 )
#endif // VOID_COMMAND_MAX_COMMANDS

struct void_command_data
{
	uint16_t                               registered_command_count;
	struct void_command_description const *registered_commands[VOID_COMMAND_MAX_COMMANDS];
	struct void_command_description const *active_modal_command;
};

static struct void_command_data void_command_instance;

static void help_command( void )
{
	struct void_command_data *command = &void_command_instance;
	for ( uint16_t i = 1; i < command->registered_command_count; ++i )
	{
		const struct void_command_description *desc = command->registered_commands[i];
		printf( "%s\r\n  ", desc->command_string );
		printf( "%s\r\n", desc->help_string );
	}
}

static const struct void_command_description help_command_description = {
    .command_string = "help",
    .command        = help_command,
    .help_string    = "The following are the currently registered commands:",
};

static void clear_command( void ) { vs_clear_console( vs_static_shell ); }

static const struct void_command_description clear_command_description = {
    .command_string = "clear",
    .command        = clear_command,
    .help_string    = "Clear the terminal",
};

void vc_init()
{
	struct void_command_data *command = &void_command_instance;
	command->registered_command_count = 0;
	command->active_modal_command     = NULL;
	vc_register( &help_command_description );
	vc_register( &clear_command_description );
	vs_text_color( COLOR_GREEN );

	vs_more_bold();
	vs_more_bold();
	printf( "  _    _  _____  _____ ______       _______ _     _ _______               \r\n" );
	printf( "   \\  /  |     |   |   |     \\      |______ |_____| |______ |      |      \r\n" );
	printf( "    \\/   |_____| __|__ |_____/      ______| |     | |______ |_____ |_____ \r\n\r\n" );

	vs_reset_format();
	vc_print_context();
}

bool vc_register( const struct void_command_description *description )
{
	struct void_command_data *command = &void_command_instance;
	if ( command->registered_command_count == VOID_COMMAND_MAX_COMMANDS )
	{
		printf( "Max Commands reached.  Please increasesID_COMMAND_MAX_COMMANDS\r\n" );
		return false;
	}
	command->registered_commands[command->registered_command_count++] = description;
	return true;
}

uint16_t vc_complete_command( char *in_out_string, uint16_t max_len )
{
	(void) ( max_len );
	struct void_command_data *command     = &void_command_instance;
	size_t                    input_len   = strlen( in_out_string );
	uint16_t                  match_index = command->registered_command_count;
	for ( uint16_t command_index = 0; command_index != command->registered_command_count;
	      ++command_index )
	{
		struct void_command_description const *desc = command->registered_commands[command_index];
		if ( input_len <= strlen( desc->command_string ) )
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
		struct void_command_description const *match   = command->registered_commands[match_index];
		uint16_t                               new_len = strlen( match->command_string );
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
	bool                      command_found = false;
	struct void_command_data *command       = &void_command_instance;
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
		printf( "Command not recognized.\r\n	Try \"help\"\r\n" );
	}
	vc_print_context();
}

void vc_print_context( void )
{
	struct void_command_data *command = &void_command_instance;
	vs_text_color( COLOR_YELLOW );
	if ( command->active_modal_command )
	{
		printf( "%s", command->active_modal_command->command_string );
	}
	printf( "$>" );
	vs_reset_format();
}
