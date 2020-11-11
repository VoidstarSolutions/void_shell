/*******************************************************************************
**
** Copyright (c) 2020 Voidstar Solutions
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
*******************************************************************************/

/*
 * void_shell.h
 *
 * Created: 06/04/20
 * Author : Zachary Heylmun
 *
 */

#include "void_shell.h"

#include <stdint.h>
#include <string.h>

#include <printf.h>

#include "void_command.h"
#include "void_shell_utilities.h"

#ifndef VOID_SHELL_ESCAPE_SEQUENCE_BUFFER_SIZE
#define VOID_SHELL_ESCAPE_SEQUENCE_BUFFER_SIZE ( (uint8_t) 8 )
#endif

#ifndef VOID_SHELL_BUFFER_SIZE
#define VOID_SHELL_BUFFER_SIZE ( (uint16_t) 512 )
#endif // VOID_SHELL_BUFFER_SIZE

#ifndef VOID_SHELL_COMMAND_HISTORY_COUNT
#define VOID_SHELL_COMMAND_HISTORY_COUNT ( (uint8_t) 8 )
#endif // VOID_SHELL_COMMAND_HISTORY_COUNT

struct command_history
{
	size_t start_index;
	size_t length;
};

struct shell_data
{
	volatile size_t        start_index;
	volatile size_t        line_length;
	volatile size_t        cursor_line;
	volatile size_t        cursor_column;
	uint8_t                escape_sequence_index;
	char                   escape_sequence_buffer[3];
	char                   shell_input_buffer[VOID_SHELL_BUFFER_SIZE];
	uint8_t                command_index;
	uint8_t                requested_command_index;
	struct command_history previous_commands[VOID_SHELL_COMMAND_HISTORY_COUNT];
};

static struct shell_data static_shell;

static void display_history_command( struct shell_data *shell )
{
	void_shell_start_of_line();
	void_shell_erase_after_cursor();
	void_command_print_context();
	struct command_history *command = &shell->previous_commands[shell->requested_command_index];
	char *history_command_start     = &shell->shell_input_buffer[command->start_index];
	char *current_command_start     = &shell->shell_input_buffer[shell->start_index];

	// TODO: Handle commands wrapping buffer
	output( history_command_start, command->length );
	memcpy( current_command_start, history_command_start, command->length );
	shell->cursor_column = command->length;
	shell->line_length   = command->length;
}

static inline void attempt_autocomplete( struct shell_data *shell )
{
	(void) ( shell );
	char *command_string = &shell->shell_input_buffer[shell->start_index];

	uint16_t updated_len = void_command_complete_command( command_string, 255 );
	if ( updated_len != 0 )
	{
		void_shell_start_of_line();
		void_command_print_context();
		shell->cursor_column = updated_len;
		shell->line_length   = updated_len;
		output( &shell->shell_input_buffer[shell->start_index], updated_len );
	}
	else
	{
		char bel = 7;
		output( &bel, 1 );
	}
}

static inline void process_command( struct shell_data *shell )
{
	// new line recieved echo a line separator
	output( "\n", 1 );
	if ( shell->line_length == 0 )
	{
		void_command_print_context();
		return;
	}

	void_shell_start_of_line();
	void_shell_erase_after_cursor();
	// null terminate the command
	shell->shell_input_buffer[shell->start_index + shell->line_length] = '\0';
	char *command_start = &shell->shell_input_buffer[shell->start_index];
	void_command_handle_command( command_start );
	shell->previous_commands[shell->command_index].start_index = shell->start_index;
	shell->previous_commands[shell->command_index].length      = shell->line_length;
	shell->command_index = ( shell->command_index + 1 ) % VOID_SHELL_COMMAND_HISTORY_COUNT;
	shell->requested_command_index = shell->command_index;
	shell->start_index =
	    ( shell->start_index + shell->line_length ) % VOID_SHELL_BUFFER_SIZE; /* Wrap if needed */
	shell->cursor_column = 0;
	shell->line_length   = 0;
	++shell->cursor_line;
}

static inline bool process_escape_sequence( struct shell_data *shell, char input_char )
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
				    ( VOID_SHELL_COMMAND_HISTORY_COUNT + shell->requested_command_index - 1 ) %
				    VOID_SHELL_COMMAND_HISTORY_COUNT;
				display_history_command( shell );
				break;
			case 'B': /* Down Arrow */
				shell->requested_command_index =
				    ( shell->requested_command_index + 1 ) % VOID_SHELL_COMMAND_HISTORY_COUNT;
				display_history_command( shell );
				break;
			case 'C': /* Right Arrow */
				if ( shell->cursor_column < shell->line_length )
				{
					void_shell_right();
					++shell->cursor_column;
				}
				break;
			case 'D': /* Left Arrow */
				if ( shell->cursor_column > 0 )
				{
					void_shell_left();
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

static inline void process_recieved_char( struct shell_data *shell, char input_char )
{
	size_t character_index = ( shell->start_index + shell->cursor_column ) % VOID_SHELL_BUFFER_SIZE;
	if ( input_char == '\b' )
	{
		if ( shell->cursor_column )
		{
			output( &input_char, 1 );
			void_shell_erase_after_cursor();
			--shell->cursor_column;
			shell->line_length = shell->cursor_column;

			shell->shell_input_buffer[--character_index] = '\0';
		}
	}
	else
	{
		output( &input_char, 1 );
		shell->shell_input_buffer[character_index] = input_char;
		++shell->cursor_column;
		if ( shell->cursor_column > shell->line_length )
		{
			shell->line_length = shell->cursor_column;
		}
	}
}

void void_shell_init( void )
{
	struct shell_data *shell       = &static_shell;
	shell->start_index             = 0;
	shell->line_length             = 0;
	shell->cursor_line             = 1;
	shell->cursor_column           = 0;
	shell->escape_sequence_index   = 0;
	shell->command_index           = 0;
	shell->requested_command_index = 0;
	memset( shell->shell_input_buffer, 0, VOID_SHELL_BUFFER_SIZE );
	memset( shell->previous_commands,
	        0,
	        sizeof( struct command_history ) * VOID_SHELL_COMMAND_HISTORY_COUNT );

	void_shell_clear();
	void_command_init();
}

void void_shell_run( void )
{
	struct shell_data *shell = &static_shell;

	int8_t input_char = void_shell_get_char();
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
			attempt_autocomplete( shell );
			break;
		case '\n':
		case '\r':
			/* newline characters, attempt to process command */
			process_command( shell );
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

void void_shell_clear()
{
	struct shell_data *shell = &static_shell;
	shell->cursor_column     = 0;
	shell->cursor_line       = 0;
	void_shell_clear_text();
	void_shell_home();
}
