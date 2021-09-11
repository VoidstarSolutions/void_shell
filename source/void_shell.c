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
 * @file void_shell.c
 *
 * @date Created: 06/04/20
 * @author Zachary Heylmun
 *
 */

#include "void_shell.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "../third_party/printf/printf.h"

#include "void_command.h"
#include "void_shell_utilities.h"

VS_STATIC struct vs_data vs_data[VS_SHELL_COUNT];

vs_handle vs_handles[VS_SHELL_COUNT];

VS_STATIC bool vs_history_entry_active( const struct vs_history_entry *entry )
{
	assert( entry->length > 0 || entry->start_index == 0 );
	return entry->length > 0;
}

/** 
 * @brief Invalidate any command completion strings that we've overrun after wrapping 
 * 
 * @param [in,out] shell shell data for which overrun history should be invalidated
 * */
VS_STATIC void vs_invalidate_history( struct vs_data *shell )
{
	for ( unsigned cmd_idx = 0; cmd_idx < VS_COMMAND_HISTORY_COUNT; ++cmd_idx )
	{
		const struct vs_history_entry *entry = &shell->previous_commands[cmd_idx];
		if ( vs_history_entry_active( entry )              // entry is active
		     && entry->start_index <= shell->start_index ) // start of entry has been overrun
		{
			memset( &shell->input_buffer[entry->start_index], '\0', entry->length );
			shell->previous_commands[cmd_idx].start_index = 0;
			shell->previous_commands[cmd_idx].length      = 0;
		}
	}
}

/**
 * @brief Handle any cleanup and wrap back to the begining of the input buffer 
 * @param [in,out] shell shell data for handling wrapped buffer
 **/
VS_STATIC void vs_buffer_wrapped( struct vs_data *shell )
{
	// copy our current command to the start of the buffer
	memcpy( shell->input_buffer, &shell->input_buffer[shell->start_index], shell->line_length );
	// erase the leftovers
	memset( &shell->input_buffer[shell->start_index], '\0', shell->line_length );
	shell->start_index = 0;
	vs_invalidate_history( shell );
}

/** 
 * @brief Display the selected history command.  If the shell is dirty, the 
 * current command will be preserved
 * @param[in,out] shell Shell data to display history from
 */
VS_STATIC void vs_display_history_command( struct vs_data *shell )
{
	vs_start_of_line( shell );
	vs_erase_after_cursor( shell );
	vc_print_context();
	if ( shell->dirty )
	{
		shell->previous_commands[shell->command_index].start_index = shell->start_index;
		shell->previous_commands[shell->command_index].length      = shell->line_length;
		// Jump ahead in the buffer to preserve what we've typed
		shell->start_index += shell->line_length;
	}

	const struct vs_history_entry *command =
	    &shell->previous_commands[shell->requested_command_index];
	const char *history_command_start = &shell->input_buffer[command->start_index];
	char *      current_command_start = &shell->input_buffer[shell->start_index];
	// check if the history command wraps the input buffer
	if ( ( current_command_start - shell->input_buffer ) + command->length >= VS_BUFFER_SIZE )
	{
		// We wrapped the input buffer, resetting to index 0
		vs_buffer_wrapped( shell );
		current_command_start = shell->input_buffer;
	}
	memset( current_command_start + command->length, '\0', shell->line_length );
	vs_output_internal( shell, history_command_start, command->length );
	memcpy( current_command_start, history_command_start, command->length );
	shell->cursor_column = command->length;
	shell->line_length   = command->length;
	shell->dirty         = false;
	vs_invalidate_history( shell );
}

/** 
 * @brief Request command completion from command processor for current command
 * @param[in,out] shell Shell Data for current command
 */
VS_STATIC void vs_attempt_autocomplete( struct vs_data *shell )
{
	(void) ( shell );
	char *command_string = &shell->input_buffer[shell->start_index];

	size_t updated_len = vc_complete_command( command_string, 255 );
	if ( updated_len != 0 )
	{
		vs_start_of_line( shell );
		vc_print_context();
		shell->cursor_column = updated_len;
		shell->line_length   = updated_len;
		vs_output_internal( shell, &shell->input_buffer[shell->start_index], updated_len );
	}
	else
	{
		char bel = 7;
		vs_output_internal( shell, &bel, 1 );
	}
}

/** 
 * @brief Evaluate the current command
 * @param[in,out] shell Shell Data for current command
 */
VS_STATIC void vs_process_command( struct vs_data *shell )
{
	size_t terminator_index = shell->start_index + shell->line_length;
	// null terminate the command
	shell->input_buffer[terminator_index] = '\0';
	// If the null terminator stomped on a history command invalidate it
	vs_invalidate_history( shell );

	const char *command_start = &shell->input_buffer[shell->start_index];
	vs_start_of_line( shell );
	vs_erase_after_cursor( shell );
	vc_print_context();
	printf( command_start );

	// new line received echo a line separator
	vs_output_internal( shell, "\n", 1 );
	vs_start_of_line( shell );
	vs_erase_after_cursor( shell );
	if ( shell->line_length == 0 )
	{
		vc_print_context();
		return;
	}

	vc_handle_command( command_start );
	shell->previous_commands[shell->command_index].start_index = shell->start_index;
	shell->previous_commands[shell->command_index].length      = shell->line_length;
	shell->command_index           = ( shell->command_index + 1 ) & VS_COMMAND_HISTORY_INDEX_MASK;
	shell->requested_command_index = shell->command_index;
	shell->start_index             = ( shell->start_index + shell->line_length );
	shell->cursor_column           = 0;
	shell->line_length             = 0;
	++shell->cursor_line;
	shell->dirty = false;
}

/** 
 * @brief Handle character as part of escape sequence
 * @param[in,out] shell Shell data for current command 
 * @param[in] input_char Potential next character in escape sequence
 * @return false if escape sequence is invalid
 **/
VS_STATIC bool process_escape_sequence( struct vs_data *shell, char input_char )
{
	shell->escape_sequence_buffer[shell->escape_sequence_index++] = input_char;
	if ( shell->escape_sequence_index == 2 && shell->escape_sequence_buffer[1] != '[' )
	{
		/* Not a valid escape sequence */
		shell->escape_sequence_index = 0;
		return false;
	}
	else if ( shell->escape_sequence_index == 3 )
	{
		switch ( shell->escape_sequence_buffer[2] )
		{
			case 'A': /* Up Arrow */
				shell->requested_command_index =
				    ( shell->requested_command_index - 1 ) & VS_COMMAND_HISTORY_INDEX_MASK;
				vs_display_history_command( shell );
				break;
			case 'B': /* Down Arrow */
				shell->requested_command_index =
				    ( shell->requested_command_index + 1 ) & VS_COMMAND_HISTORY_INDEX_MASK;
				vs_display_history_command( shell );
				break;
			case 'C': /* Right Arrow */
				if ( shell->cursor_column < shell->line_length )
				{
					vs_right( shell );
					++shell->cursor_column;
				}
				break;
			case 'D': /* Left Arrow */
				if ( shell->cursor_column > 0 )
				{
					vs_left( shell );
					--shell->cursor_column;
				}
				break;
			default:
				break;
		}
		shell->escape_sequence_index = 0;
	}
	return true;
}

/**
 * @brief Handle a received character
 * 
 * @param [in,out] shell Shell data to add received character to
 * @param [in] input_char received character to process
 */
VS_STATIC void process_received_char( struct vs_data *shell, char input_char )
{
	size_t character_index = ( shell->start_index + shell->cursor_column ) & VS_BUFFER_INDEX_MASK;

	if ( input_char == '\b' || /* backspace */
	     input_char == 0x7f /* delete (for Mac)*/ )
	{
		input_char = '\b';
		if ( shell->cursor_column )
		{
			vs_output_internal( shell, &input_char, 1 );
			vs_erase_after_cursor( shell );
			--shell->cursor_column;
			shell->line_length                     = shell->cursor_column;
			shell->input_buffer[--character_index] = '\0';
			shell->dirty                           = true;
		}
	}
	else
	{
		if ( shell->cursor_column && character_index == 0 )
		{
			vs_buffer_wrapped( shell );
			character_index = shell->cursor_column;
		}
		vs_output_internal( shell, &input_char, 1 );
		shell->input_buffer[character_index++] = input_char;
		shell->input_buffer[character_index]   = '\0';
		++shell->cursor_column;
		if ( shell->cursor_column > shell->line_length )
		{
			shell->line_length = shell->cursor_column;
		}
		shell->dirty = true;
		vs_invalidate_history( shell );
	}
}

void vs_init()
{
	for ( unsigned shell_index = 0; shell_index < VS_SHELL_COUNT; ++shell_index )
	{
		vs_handle shell         = &vs_data[shell_index];
		vs_handles[shell_index] = shell;
		memset( shell, 0, sizeof( struct vs_data ) );
		vs_clear_console( shell );
	}
}

void vs_configure( vs_handle   shell,
                   vs_get_char input_func,
                   vs_output   output_func,
                   bool        echo_enabled )
{
	shell->input_func   = input_func;
	shell->output_func  = output_func;
	shell->echo_enabled = echo_enabled;
}

void vs_run( vs_handle shell )
{

	int8_t input_char = shell->input_func();
	if ( input_char < 0 )
	{
		return; /* for non-blocking superloop operation */
	}

	if ( shell->escape_sequence_index                      /* If an escape sequence is active */
	     && process_escape_sequence( shell, input_char ) ) /* and it is not aborted */
	{
		return;
	}

	switch ( input_char )
	{
		case 9: /* tab */
			vs_attempt_autocomplete( shell );
			break;
		case '\n':
		case '\r':
			/* newline characters, attempt to process command */
			vs_process_command( shell );
			break;
		case 27: /* Escape Character */
			/* start of a new escape sequence */
			process_escape_sequence( shell, input_char );
			break;
		default:
			/* Handle non-command characters */
			process_received_char( shell, input_char );
			break;
	}
}

void vs_clear_console( vs_handle shell )
{
	shell->cursor_column = 0;
	shell->cursor_line   = 0;
	vs_clear_text( shell );
	vs_home( shell );
}

void vs_output_internal( const_vs_handle shell, const char *data, size_t length )
{
	if ( shell->echo_enabled )
	{
		shell->output_func( data, length );
	}
}
