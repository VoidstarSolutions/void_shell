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
 * \file void_shell.c
 *
 * \date Created: 06/04/20
 * \author Zachary Heylmun
 *
 */

#include "void_shell.h"

#include <stdint.h>
#include <string.h>

#include <printf.h>

#include "void_command.h"
#include "void_shell_utilities.h"

#ifndef VS_BUFFER_SIZE_POW_TWO
/** Default buffer size of 2^8, or 256 characters */
#define VS_BUFFER_SIZE_POW_TWO ( (uint8_t) 8 )
#endif // VS_BUFFER_SIZE_POW_TWO

#ifndef VS_COMMAND_HISTORY_COUNT_POW_TWO
/** Default history length of 2^2, or 4 commands */
#define VS_COMMAND_HISTORY_COUNT_POW_TWO ( (uint8_t) 2 )
#endif // VS_COMMAND_HISTORY_COUNT_POW_TWO

#define VS_ESCAPE_SEQUENCE_BUFFER_SIZE ( (uint8_t) 4 )
#define VS_BUFFER_SIZE ( (unsigned) 1 << VS_BUFFER_SIZE_POW_TWO )
#define VS_BUFFER_INDEX_MASK ( (unsigned) VS_BUFFER_SIZE - 1 )
#define VS_COMMAND_HISTORY_COUNT ( (unsigned) 1 << VS_COMMAND_HISTORY_COUNT_POW_TWO )
#define VS_COMMAND_HISTORY_INDEX_MASK ( (unsigned) VS_COMMAND_HISTORY_COUNT - 1 )

/** Struct to hold start and length of previous commands */
struct vs_command_history_entry
{
	size_t start_index;
	size_t length;
};

/** Struct to hold shell data */
struct vs_shell_data
{
	/** Index of first character in active command */
	volatile size_t start_index;
	/** Number of characters in active command */
	volatile size_t line_length;
	/** Column of user cursor.  May be less than line_length */
	volatile size_t cursor_column;
	/** Line number of cursor in terminal */
	volatile size_t cursor_line;
	/** Index in escape buffer.  0 if no escape sequence active */
	volatile size_t escape_sequence_index;
	/** Buffer to capture escape sequences outside of character buffer */
	char escape_sequence_buffer[VS_ESCAPE_SEQUENCE_BUFFER_SIZE];
	/** Input buffer.  Stores active command as well as history */
	char input_buffer[VS_BUFFER_SIZE];
	/** Index of current command in history buffer */
	size_t command_index;
	/** Index of requested command for accessing historical commands */
	size_t requested_command_index;
	/** Buffer to store information about past commands */
	struct vs_command_history_entry previous_commands[VS_COMMAND_HISTORY_COUNT];
	/** Shell is dirty if user has entered characters other than control characters */
	bool dirty;
};

static struct vs_shell_data static_shell;

/** \brief Invalidate any command completion strings that we've overrun after wrapping */
static void vs_invalidate_history( struct vs_shell_data *shell )
{
	for ( unsigned cmd_idx = 0; cmd_idx < VS_COMMAND_HISTORY_COUNT; cmd_idx++ )
	{
		struct vs_command_history_entry *command = &shell->previous_commands[cmd_idx];
		if ( command->length && command->start_index >= shell->start_index &&
		     command->start_index < ( shell->start_index + shell->line_length + 1 ) )
		{
			memset( &shell->input_buffer[command->start_index], '\0', command->length );
			shell->previous_commands[cmd_idx].start_index = 0;
			shell->previous_commands[cmd_idx].length      = 0;
		}
	}
}

static void vs_buffer_wrapped( struct vs_shell_data *shell )
{
	// copy our current command to the start of the buffer
	memcpy( shell->input_buffer, &shell->input_buffer[shell->start_index], shell->line_length );
	// erase the leftovers
	memset( &shell->input_buffer[shell->start_index], '\0', shell->line_length );
	shell->start_index   = 0;
	shell->cursor_column = shell->line_length;
	vs_invalidate_history( shell );
}

static void vs_display_history_command( struct vs_shell_data *shell )
{
	vs_start_of_line();
	vs_erase_after_cursor();
	vc_print_context();
	if ( shell->dirty )
	{
		shell->previous_commands[shell->command_index].start_index = shell->start_index;
		shell->previous_commands[shell->command_index].length      = shell->line_length;
		// Jump ahead in the buffer to preserve what we've typed
		shell->start_index += shell->line_length;
	}

	struct vs_command_history_entry *command =
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
	// TODO: Handle commands wrapping buffer
	vs_output( history_command_start, command->length );
	memcpy( current_command_start, history_command_start, command->length );
	shell->cursor_column = command->length;
	shell->line_length   = command->length;
	shell->dirty         = false;
	vs_invalidate_history( shell );
}

static inline void vs_attempt_autocomplete( struct vs_shell_data *shell )
{
	(void) ( shell );
	char *command_string = &shell->input_buffer[shell->start_index];

	uint16_t updated_len = vc_complete_command( command_string, 255 );
	if ( updated_len != 0 )
	{
		vs_start_of_line();
		vc_print_context();
		shell->cursor_column = updated_len;
		shell->line_length   = updated_len;
		vs_output( &shell->input_buffer[shell->start_index], updated_len );
	}
	else
	{
		char bel = 7;
		vs_output( &bel, 1 );
	}
}

static inline void vs_process_command( struct vs_shell_data *shell )
{
	size_t terminator_index = shell->start_index + shell->line_length;
	// null terminate the command
	shell->input_buffer[terminator_index] = '\0';
	// If the null terminator stomped on a history command invalidate it
	vs_invalidate_history( shell );

	char *command_start = &shell->input_buffer[shell->start_index];
	vs_start_of_line();
	vs_erase_after_cursor();
	vc_print_context();
	printf( command_start );

	// new line recieved echo a line separator
	vs_output( "\n", 1 );
	vs_start_of_line();
	vs_erase_after_cursor();
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

static inline bool process_escape_sequence( struct vs_shell_data *shell, char input_char )
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
					vs_right();
					++shell->cursor_column;
				}
				break;
			case 'D': /* Left Arrow */
				if ( shell->cursor_column > 0 )
				{
					vs_left();
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

static inline void process_recieved_char( struct vs_shell_data *shell, char input_char )
{
	size_t character_index = ( shell->start_index + shell->cursor_column ) & VS_BUFFER_INDEX_MASK;

	if ( input_char == '\b' || /* backspace */
	     input_char == 0x7f /* delete (for Mac)*/ )
	{
		input_char = '\b';
		if ( shell->cursor_column )
		{
			vs_output( &input_char, 1 );
			vs_erase_after_cursor();
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
		vs_output( &input_char, 1 );
		shell->input_buffer[character_index] = input_char;
		++shell->cursor_column;
		if ( shell->cursor_column > shell->line_length )
		{
			shell->line_length = shell->cursor_column;
		}
		shell->dirty = true;
		vs_invalidate_history( shell );
	}
}

void vs_init( void )
{
	struct vs_shell_data *shell    = &static_shell;
	shell->start_index             = 0;
	shell->line_length             = 0;
	shell->cursor_line             = 1;
	shell->cursor_column           = 0;
	shell->escape_sequence_index   = 0;
	shell->command_index           = 0;
	shell->requested_command_index = 0;
	memset( shell->input_buffer, 0, VS_BUFFER_SIZE );
	memset( shell->previous_commands,
	        0,
	        sizeof( struct vs_command_history_entry ) * VS_COMMAND_HISTORY_COUNT );

	vs_clear_console();
	vc_init();
}

void vs_run( void )
{
	struct vs_shell_data *shell = &static_shell;

	int8_t input_char = vs_get_char();
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
			process_recieved_char( shell, input_char );
			break;
	}
}

void vs_clear_console()
{
	struct vs_shell_data *shell = &static_shell;
	shell->cursor_column        = 0;
	shell->cursor_line          = 0;
	vs_clear_text();
	vs_home();
}
