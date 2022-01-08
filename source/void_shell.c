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
#include <string.h>

#include "void_command.h"
#include "void_shell_internal.h"
#include "void_shell_utilities.h"


VS_STATIC struct vs_data shell;

VS_STATIC_INLINE bool vs_history_entry_active( const struct vs_history_entry *entry )
{
    assert( entry->length > 0 || entry->start_index == 0 );
    uint16_t test = UINT16_MAX;
    return entry->length > 0;
}

/**
 * @brief Invalidate any command completion strings that we've overrun
 *
 * @param [in,out] shell shell data for which overrun history should be invalidated
 * */
VS_STATIC void vs_invalidate_history( void )
{
    for ( unsigned cmd_idx = 0; cmd_idx < VS_COMMAND_HISTORY_COUNT; ++cmd_idx )
    {
        const struct vs_history_entry *entry = &shell.previous_commands[cmd_idx];
        if ( vs_history_entry_active( entry )           // entry is active
             && shell.line_length > 0                   // the line has overwritten anything
             && entry->start_index <= shell.start_index // entry starts before current command
             && entry->start_index + entry->length > shell.start_index // and ends after it starts
        )
        {
            memset( &shell.input_buffer[entry->start_index], '\0', entry->length );
            shell.previous_commands[cmd_idx].start_index = 0;
            shell.previous_commands[cmd_idx].length      = 0;
        }
    }
}

/**
 * @brief Handle any cleanup and wrap back to the begining of the input buffer
 * @param [in,out] shell shell data for handling wrapped buffer
 **/
VS_STATIC void vs_buffer_wrapped( void )
{
    // copy our current command to the start of the buffer
    memcpy( shell.input_buffer, &shell.input_buffer[shell.start_index], shell.line_length );
    // erase the leftovers
    memset( &shell.input_buffer[shell.start_index], '\0', shell.line_length );
    shell.start_index = 0;
    vs_invalidate_history();
}

/**
 * @brief Display the selected history command.  If the shell is dirty, the
 * current command will be preserved
 * @param[in,out] shell Shell data to display history from
 */
VS_STATIC void vs_display_history_command( void )
{
    vs_start_of_line();
    vs_erase_after_cursor();

    vc_print_context(true);
    if ( shell.dirty )
    {
        shell.previous_commands[shell.command_index].start_index = shell.start_index;
        shell.previous_commands[shell.command_index].length      = shell.line_length;
        // Jump ahead in the buffer to preserve what we've typed
        shell.start_index += shell.line_length;
        // This can't wrap, or we would have already handled it, but just in case
        assert( shell.start_index < VS_BUFFER_SIZE );
    }

    const struct vs_history_entry *requested_command =
        &shell.previous_commands[shell.requested_command_index];
    // check if the history command wraps the input buffer
    if ( shell.start_index + requested_command->length >= VS_BUFFER_SIZE )
    {
        // We wrapped the input buffer, res=etting to index 0
        vs_buffer_wrapped();
    }
    memset( &shell.input_buffer[shell.start_index], '\0', shell.line_length );
    vs_output( &shell.input_buffer[requested_command->start_index], requested_command->length );

    memcpy( &shell.input_buffer[shell.start_index],
            &shell.input_buffer[requested_command->start_index],
            requested_command->length );
    shell.command_index = shell.requested_command_index;
    shell.cursor_column = requested_command->length;
    shell.line_length   = requested_command->length;
    shell.dirty         = false;
}

/**
 * @brief Request command completion from command processor for current command
 * @param[in,out] shell Shell Data for current command
 */
VS_STATIC_INLINE void vs_attempt_autocomplete( void )
{
    char *command_string = &shell.input_buffer[shell.start_index];
    // Check if the command is a substring, but don't modify the input buffer yet
    size_t required_size = vc_complete_command( command_string, shell.line_length, false );
    assert( required_size < VS_BUFFER_SIZE );

    if ( required_size != 0 )
    {
        // Check if the command completion is too long without wrapping
        if ( required_size > VS_BUFFER_SIZE - shell.start_index )
        {
            vs_buffer_wrapped();
            command_string = &shell.input_buffer[shell.start_index];
        }
        // Update the buffer this time
        size_t updated_len = vc_complete_command( command_string, shell.line_length, true );
        vs_start_of_line();
        vc_print_context(true);
        shell.cursor_column = updated_len;
        shell.line_length   = updated_len;
        vs_output( &shell.input_buffer[shell.start_index], updated_len );
    }
    else
    {
        char bel = 7;
        vs_output( &bel, 1 );
    }
}

/**
 * @brief Evaluate the current command
 * @param[in,out] shell Shell Data for current command
 */
VS_STATIC void vs_process_command( void )
{
    size_t terminator_index = shell.start_index + shell.line_length;

    // null terminate the command
    shell.input_buffer[terminator_index] = '\0';
    ++shell.line_length;
    // If the null terminator stomped on a history command invalidate it
    vs_invalidate_history();

    const char *command_start = &shell.input_buffer[shell.start_index];
    vs_start_of_line();
    vs_erase_after_cursor();
    vc_print_context(false);
    vs_output( command_start, strlen(command_start) );

    // new line received echo a line separator
    vs_output( "\n", 1 );
    vs_start_of_line();
    vs_erase_after_cursor();
    if ( shell.line_length == 0 )
    {
        vc_print_context(true);
        return;
    }

    vc_handle_command( command_start );
    shell.previous_commands[shell.command_index].start_index = shell.start_index;
    shell.previous_commands[shell.command_index].length      = shell.line_length;
    shell.command_index           = ( shell.command_index + 1 ) & VS_COMMAND_HISTORY_INDEX_MASK;
    shell.requested_command_index = shell.command_index;
    shell.start_index             = ( shell.start_index + shell.line_length );
    shell.cursor_column           = 0;
    shell.line_length             = 0;
    ++shell.cursor_line;
    shell.dirty = false;
}

/**
 * @brief Handle character as part of escape sequence
 * @param[in,out] shell Shell data for current command
 * @param[in] input_char Potential next character in escape sequence
 * @return false if escape sequence is invalid
 **/
VS_STATIC bool process_escape_sequence( char input_char )
{
    shell.escape_sequence_buffer[shell.escape_sequence_index++] = input_char;
    if ( shell.escape_sequence_index == 2 && shell.escape_sequence_buffer[1] != '[' )
    {
        /* Not a valid escape sequence */
        shell.escape_sequence_index = 0;
        return false;
    }
    else if ( shell.escape_sequence_index == 3 )
    {
        switch ( shell.escape_sequence_buffer[2] )
        {
            case 'A': /* Up Arrow */
                shell.requested_command_index =
                    ( shell.requested_command_index - 1 ) & VS_COMMAND_HISTORY_INDEX_MASK;
                vs_display_history_command();
                break;
            case 'B': /* Down Arrow */
                shell.requested_command_index =
                    ( shell.requested_command_index + 1 ) & VS_COMMAND_HISTORY_INDEX_MASK;
                vs_display_history_command();
                break;
            case 'C': /* Right Arrow */
                if ( shell.cursor_column < shell.line_length )
                {
                    vs_right();
                    ++shell.cursor_column;
                }
                break;
            case 'D': /* Left Arrow */
                if ( shell.cursor_column > 0 )
                {
                    vs_left();
                    --shell.cursor_column;
                }
                break;
            default:
                break;
        }
        shell.escape_sequence_index = 0;
    }
    return true;
}

/**
 * @brief Handle a received character
 *
 * @param [in,out] shell Shell data to add received character to
 * @param [in] input_char received character to process
 */
VS_STATIC_INLINE void process_received_char( char input_char )
{
    size_t character_index = ( shell.start_index + shell.cursor_column ) & VS_BUFFER_INDEX_MASK;

    if ( input_char == '\b' || /* backspace */
         input_char == 0x7f /* delete (for Mac)*/ )
    {
        input_char = '\b';
        if ( shell.cursor_column )
        {
            vs_output( &input_char, 1 );
            vs_erase_after_cursor();
            --shell.cursor_column;
            shell.line_length                     = shell.cursor_column;
            shell.input_buffer[--character_index] = '\0';
            shell.dirty                           = true;
        }
    }
    else
    {
        if ( shell.cursor_column && character_index == 0 )
        {
            vs_buffer_wrapped();
            character_index = shell.cursor_column;
        }
        vs_output( &input_char, 1 );
        shell.input_buffer[character_index++] = input_char;
        shell.input_buffer[character_index]   = '\0';
        ++shell.cursor_column;
        if ( shell.cursor_column > shell.line_length )
        {
            shell.line_length = shell.cursor_column;
        }
        shell.dirty = true;
        vs_invalidate_history();
    }
}

void vs_configure( vs_get_char input_func, vs_put_char output_func )
{
    memset( &shell, 0, sizeof( struct vs_data ) );
    shell.input_func  = input_func;
    shell.output_func = output_func;
}

void vs_run( void )
{

    int8_t input_char = shell.input_func();
    if ( input_char < 0 )
    {
        return; /* for non-blocking superloop operation */
    }

    if ( shell.escape_sequence_index                /* If an escape sequence is active */
         && process_escape_sequence( input_char ) ) /* and it is not aborted */
    {
        return;
    }

    switch ( input_char )
    {
        case 9: /* tab */
            vs_attempt_autocomplete();
            break;
        case '\n':
        case '\r':
            /* newline characters, attempt to process command */
            vs_process_command();
            break;
        case 27: /* Escape Character */
            /* start of a new escape sequence */
            process_escape_sequence( input_char );
            break;
        default:
            /* Handle non-command characters */
            process_received_char( input_char );
            break;
    }
}

void vs_clear_console( void )
{
    shell.cursor_column = 0;
    shell.cursor_line   = 0;
    vs_clear_text();
    vs_home();
}

void vs_output( const char *data, size_t length )
{
    for ( int index = 0; index < length; ++index )
    {
	  shell.output_func( data[index] );
    }
}
