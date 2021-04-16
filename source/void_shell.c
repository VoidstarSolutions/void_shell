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

#include <stdint.h>
#include <string.h>

#include <printf.h>

#include "void_command.h"
#include "void_shell_utilities.h"

#define VS_ESCAPE_SEQUENCE_BUFFER_SIZE ( (uint8_t) 4 )
#define VS_BUFFER_SIZE ( (unsigned) 1 << VS_BUFFER_SIZE_POW_TWO )
#define VS_BUFFER_INDEX_MASK ( (unsigned) VS_BUFFER_SIZE - 1 )
#define VS_COMMAND_HISTORY_COUNT ( (unsigned) 1 << VS_COMMAND_HISTORY_COUNT_POW_TWO )
#define VS_COMMAND_HISTORY_INDEX_MASK ( (unsigned) VS_COMMAND_HISTORY_COUNT - 1 )

/** 
 * @brief Struct to hold start and length of previous commands 
 * **/
struct vs_command_history_entry
{
	size_t start_index;
	size_t length;
};

/** 
 * @brief Struct to hold shell data 
 **/
struct vs_shell_data
{
	/** Function to retrieve next input character */
	vs_get_char input_func;
	/** Function for shell output */
	vs_output output_func;
	/** Index of first character in active command */
	size_t start_index;
	/** Number of characters in active command */
	size_t line_length;
	/** Column of user cursor.  May be less than line_length */
	size_t cursor_column;
	/** Line number of cursor in terminal */
	size_t cursor_line;
	/** Index in escape buffer.  0 if no escape sequence active */
	size_t escape_sequence_index;
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
	/** Whether this shell will echo typed characters */
	bool echo_enabled;
	/** Shell is dirty if user has entered characters other than control characters */
	bool dirty;
};

static struct vs_shell_data void_shell_data[VS_SHELL_COUNT];

vs_shell_handle vs_shell_handles[VS_SHELL_COUNT];

/** 
 * @brief Invalidate any command completion strings that we've overrun after wrapping 
 * 
 * @param [in,out] shell shell data for which overrun history should be invalidated
 * */
static void vs_invalidate_history( struct vs_shell_data *shell )
{
	for ( unsigned cmd_idx = 0; cmd_idx < VS_COMMAND_HISTORY_COUNT; cmd_idx++ )
	{
		const struct vs_command_history_entry *command = &shell->previous_commands[cmd_idx];
		if ( command->length && command->start_index >= shell->start_index &&
		     command->start_index <= ( shell->start_index + shell->line_length  ) )
		{
			memset( &shell->input_buffer[command->start_index], '\0', command->length );
			shell->previous_commands[cmd_idx].start_index = 0;
			shell->previous_commands[cmd_idx].length      = 0;
		}
	}
}

/**
 * @brief Handle any cleanup and wrap back to the begining of the input buffer 
 * @param [in,out] shell shell data for handling wrapped buffer
 **/
static void vs_buffer_wrapped( struct vs_shell_data *shell )
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
static void vs_display_history_command( struct vs_shell_data *shell )
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

	const struct vs_command_history_entry *command =
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
static inline void vs_attempt_autocomplete( struct vs_shell_data *shell )
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
static inline void vs_process_command( struct vs_shell_data *shell )
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

	// new line recieved echo a line separator
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
 * @brief Handle a recieved character
 * 
 * @param [in,out] shell Shell data to add recieved character to
 * @param [in] input_char recieved character to process
 */
static inline void process_recieved_char( struct vs_shell_data *shell, char input_char )
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
		shell->input_buffer[character_index] = '\0';
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
		vs_shell_handle shell          = &void_shell_data[shell_index];
		vs_shell_handles[shell_index]  = shell;
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
		vs_clear_console( shell );
	}
}

void vs_configure( vs_shell_handle shell,
                   vs_get_char     input_func,
                   vs_output       output_func,
                   bool            echo_enabled )
{
	shell->input_func   = input_func;
	shell->output_func  = output_func;
	shell->echo_enabled = echo_enabled;
}

void vs_run( vs_shell_handle shell )
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
			process_recieved_char( shell, input_char );
			break;
	}
}

void vs_clear_console( vs_shell_handle shell )
{
	shell->cursor_column = 0;
	shell->cursor_line   = 0;
	vs_clear_text( shell );
	vs_home( shell );
}

void vs_output_internal( const vs_shell_handle shell, const char *data, size_t length )
{
	if ( shell->echo_enabled )
	{
		shell->output_func( data, length );
	}
}
