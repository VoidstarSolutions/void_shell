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
 * \file void_shell_internal.h
 * \brief Module providing shell functionality
 * \date created: 06/04/20
 * \author Zachary Heylmun
 */

#ifndef VOID_SHELL_INTERNAL_H
#define VOID_SHELL_INTERNAL_H

#include <stdint.h>

#ifndef VS_SHELL_COUNT
/** Default is a single shell */
#define VS_SHELL_COUNT ( (size_t) 1 )
#endif // VS_SHELL_COUNT

#ifndef VS_BUFFER_SIZE_POW_TWO
/** Default buffer size of 2^8, or 256 characters */
#define VS_BUFFER_SIZE_POW_TWO ( (size_t) 8 )
#endif // VS_BUFFER_SIZE_POW_TWO

#ifndef VS_COMMAND_HISTORY_COUNT_POW_TWO
/** Default history length of 2^2, or 4 commands */
#define VS_COMMAND_HISTORY_COUNT_POW_TWO ( (size_t) 2 )
#endif // VS_COMMAND_HISTORY_COUNT_POW_TWO

#define VS_ESCAPE_SEQUENCE_BUFFER_SIZE ( (uint8_t) 4 )
#define VS_BUFFER_SIZE ( (unsigned) 1 << VS_BUFFER_SIZE_POW_TWO )
#define VS_BUFFER_INDEX_MASK ( (unsigned) VS_BUFFER_SIZE - 1 )
#define VS_COMMAND_HISTORY_COUNT ( (unsigned) 1 << VS_COMMAND_HISTORY_COUNT_POW_TWO )
#define VS_COMMAND_HISTORY_INDEX_MASK ( (unsigned) VS_COMMAND_HISTORY_COUNT - 1 )

// Allow unit tests to call static functions
#ifndef __TEST__
#define VS_STATIC static
#else
#define VS_STATIC
#endif

/**
 * @brief Function to get next input character for shell
 * Must be provided by console application
 * @return Negative if not available, or ASCII value from 0-127
 **/
typedef int8_t ( *vs_get_char )();

/**
 * @brief Ouput function to be used by shell
 * Must be provided by console application
 **/
typedef void ( *vs_output )( const char *data, size_t length );

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
 * Note that this data should all be considered private.
 * Never directly access or modify the members
 **/
struct vs_data
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

#endif // VOID_SHELL_INTERNAL_H