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
 * @file void_shell_internal.h
 * @brief Internal definitions for void_shell
 * @date created: 1/08/22
 * @author Zachary Heylmun
 */

#ifndef VOID_SHELL_INTERNAL_H
#define VOID_SHELL_INTERNAL_H


#define VS_ESCAPE_SEQUENCE_BUFFER_SIZE ( (size_t) 4 )
#define VS_BUFFER_SIZE ( (size_t) 1 << VS_BUFFER_SIZE_POW_TWO )
#define VS_BUFFER_INDEX_MASK ( (size_t) VS_BUFFER_SIZE - 1 )
#define VS_COMMAND_HISTORY_COUNT ( (size_t) 1 << VS_COMMAND_HISTORY_COUNT_POW_TWO )
#define VS_COMMAND_HISTORY_INDEX_MASK ( (size_t) VS_COMMAND_HISTORY_COUNT - 1 )

/**
 * @brief Struct to hold start and length of previous commands
 * **/
struct vs_history_entry
{
    /** buffer index of first character */
    size_t start_index;
    /** Length of command */
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
    vs_put_char output_func;
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
    struct vs_history_entry previous_commands[VS_COMMAND_HISTORY_COUNT];
    /** Shell is dirty if user has entered characters other than control characters */
    bool dirty;
};

VS_STATIC struct vs_data shell;

#endif // VOID_SHELL_INTERNAL_H
