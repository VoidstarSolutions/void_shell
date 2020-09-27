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

#ifndef VOID_SHELL_ESCAPE_SEQUENCE_BUFFER_SIZE
#define VOID_SHELL_ESCAPE_SEQUENCE_BUFFER_SIZE ( (uint8_t) 8 )
#endif

#ifndef VOID_SHELL_BUFFER_SIZE
#define VOID_SHELL_BUFFER_SIZE ( (uint16_t) 512 )
#endif // VOID_SHELL_BUFFER_SIZE

static const char new_line[]      = { '\r', '\n' };
static const char erase_display[] = { 27, 91, 50, 74 };
static const char erase_line[]    = { 27, 91, 75 };
static const char cursor_up[]     = { 27, 91, 65 };
static const char cursor_down[]   = { 27, 91, 66 };

struct shell_data
{
	volatile uint16_t start_index;
	volatile uint16_t end_index;
	volatile uint16_t cursor_index;
	uint8_t           escape_sequence_index;
	char              escape_sequence_buffer[16];

	char shell_input_buffer[VOID_SHELL_BUFFER_SIZE];
};

static struct shell_data static_shell;

static inline void process_command( struct shell_data *shell )
{
	shell->cursor_index = 0;
	shell->end_index = 0;
	// new line recieved echo a line separator
	printf( new_line );
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
	else if( shell->escape_sequence_index == 3 )
	{
		switch (shell->escape_sequence_buffer[2])
		{
		case 'A':
			printf(cursor_down);
			printf("Up Arrow Pressed");
			break;
		case 'B':
			printf(cursor_up);
			printf("Down Arrow Pressed");
			break;
		default:
			break;
		}
	}
	return true;
}

static inline void process_recieved_char( struct shell_data *shell, char input_char )
{
	printf( "%c", input_char );
	if ( input_char == '\b' )
	{
		printf( erase_line );
		if ( shell->cursor_index > shell->start_index )
		{
			--shell->cursor_index;
			shell->shell_input_buffer[shell->cursor_index] = '\0';
		}
	}
	else
	{
		if ( shell->cursor_index )
		{
			shell->shell_input_buffer[shell->cursor_index] = input_char;
			++shell->cursor_index;
			if ( shell->cursor_index > shell->end_index )
			{
				shell->end_index = shell->cursor_index;
			}
		}
	}
}

void void_shell_run( void )
{
	struct shell_data *shell = &static_shell;
	char               input_char;
	if ( !void_shell_get_char( &input_char ) )
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
		case '\n':
		case '\r':
			/* newline characters, attempt to process command */
			process_command( shell );
			break;
		case 27:
			/* start of a new escape sequence */
			process_escape_sequence( shell, input_char );
			break;
		default:
			/* Handle non-command characters */
			process_recieved_char( shell, input_char );
			break;
	}
}
