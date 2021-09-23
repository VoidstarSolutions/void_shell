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

#include "../third_party/printf/printf.h"

#include "void_command_default_commands.h"
#include "void_shell.h"
#include "void_shell_utilities.h"

static struct vc_data  instance;
struct vc_data *vc = &instance;

void vc_init()
{
	vc->registered_command_count = 0;
	vc->active_modal_command     = NULL;
#if ( VC_DEFAULT_COMMANDS == 1 )
	vc_register( &default_help_command_description );
	vc_register( &default_clear_command_description );
#endif //VC_DEFAULT_COMMANDS
}

void vc_activate( const_vs_handle shell )
{
	vc_print_greeting( shell );
	vc_print_context( shell );
}

bool vc_register( const struct vc_description *description )
{
	assert( strnlen( description->command_string, VC_MAX_COMMAND_LEN + 1 ) <= VC_MAX_COMMAND_LEN );
	if ( vc->registered_command_count == VC_MAX_COMMANDS )
	{
		printf( "Max Commands reached.  Please increasesID_COMMAND_MAX_COMMANDS\r\n" );
		return false;
	}
	vc->registered_commands[vc->registered_command_count++] = description;
	return true;
}

size_t vc_completion_available( const char *partial_command, size_t input_len, size_t max_len )
{
	(void) ( partial_command );
	(void) ( input_len );
	(void) ( max_len );
	//const struct vc_data *command = &void_command_instance;
	return 0;
}

size_t vc_complete_command( char *in_out_string, size_t input_len, size_t max_len )
{
	(void) ( max_len );
	uint16_t match_index = vc->registered_command_count;
	for ( uint16_t command_index = 0; command_index != vc->registered_command_count;
	      ++command_index )
	{
		struct vc_description const *desc = vc->registered_commands[command_index];
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
				if ( match_index == vc->registered_command_count )
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
	if ( match_index != vc->registered_command_count )
	{
		struct vc_description const *match   = vc->registered_commands[match_index];
		size_t                       new_len = strnlen( match->command_string, VC_MAX_COMMAND_LEN );
		for ( uint16_t char_index = 0; char_index != new_len; ++char_index )
		{
			in_out_string[char_index] = match->command_string[char_index];
		}
		return new_len;
	}
	return 0;
}

void vc_handle_command( const_vs_handle shell, const char *command_string )
{
	bool command_found = false;
	for ( uint16_t command_index = 0; command_index != vc->registered_command_count;
	      ++command_index )
	{
		if ( strcmp( vc->registered_commands[command_index]->command_string, command_string ) == 0 )
		{
			vc->registered_commands[command_index]->command( 0, NULL );
			command_found = true;
			break;
		}
	}
	if ( !command_found )
	{
		printf( "Command not recognized.\r\n\tTry \"help\"\r\n" );
	}
	vc_print_context( shell );
}

void vc_print_context( const_vs_handle shell )
{
	vs_text_color( shell, COLOR_YELLOW );
	if ( vc->active_modal_command )
	{
		printf( "%s", vc->active_modal_command->command_string );
	}
	printf( "$>" );
	vs_reset_format( shell );
}
