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

#include "void_shell.h"
#include "void_shell_utilities.h"

struct vc_data
{
    uint16_t                     registered_command_count;
    struct vc_description const *registered_commands[VC_MAX_COMMANDS];
    struct vc_description const *active_modal_command;
};

static struct vc_data instance;
struct vc_data       *vc = &instance;

static void help_command( int arg_c, char **arg_v )
{
    (void) ( arg_c );
    (void) ( arg_v );
    for ( uint16_t i = 1; i < vc->registered_command_count; ++i )
    {
        const struct vc_description *desc = vc->registered_commands[i];
        vs_output( desc->command_string, strlen( desc->command_string ) );
        vs_output( vs_separator, strlen( vs_separator ) );
        vs_output( desc->help_string, strlen( desc->help_string ) );
        vs_output( vs_newline, strlen( vs_newline ) );
    }
}

const struct vc_description help_command_description = {
    .command_string = "help",
    .command        = help_command,
    .help_string    = "The following are the currently registered commands:",
};

static void clear_command( int argc, char **argv )
{
    (void) ( argc );
    (void) ( argv );
    vs_clear_console();
}

const struct vc_description clear_command_description = {
    .command_string = "clear",
    .command        = clear_command,
    .help_string    = "Clear the terminal",
};

VS_STATIC bool
vc_input_matches( const char *input, size_t input_len, const struct vc_description *command )
{
    bool match = false;
    if ( input_len <= strnlen( command->command_string, VC_MAX_COMMAND_LEN ) )
    {
        match = true;
        for ( uint16_t char_index = 0; char_index != input_len; ++char_index )
        {
            if ( input[char_index] != command->command_string[char_index] )
            {
                match = false;
            }
        }
    }
    return match;
}

void vc_init()
{
    vc->registered_command_count = 0;
    vc->active_modal_command     = NULL;
#if ( VC_DEFAULT_COMMANDS == 1 )
    vc_register( &help_command_description );
    vc_register( &clear_command_description );
#endif //VC_DEFAULT_COMMANDS
    vc_print_greeting();
    vc_print_context( true );
}

bool vc_register( const struct vc_description *description )
{
    assert( strnlen( description->command_string, VC_MAX_COMMAND_LEN + 1 ) <= VC_MAX_COMMAND_LEN );
    if ( vc->registered_command_count == VC_MAX_COMMANDS )
    {
        return false;
    }
    vc->registered_commands[vc->registered_command_count++] = description;
    return true;
}

size_t vc_complete_command( char *in_out_string, size_t input_len, bool modify_buffer )
{
    uint16_t command_index = 0;
    for ( ; command_index != vc->registered_command_count; ++command_index )
    {
        struct vc_description const *command = vc->registered_commands[command_index];
        if ( vc_input_matches( in_out_string, input_len, command ) )
        {
            // We've found a matching command, stop searching
            break;
        }
    }
    if ( command_index != vc->registered_command_count )
    {
        struct vc_description const *match   = vc->registered_commands[command_index];
        size_t                       new_len = strnlen( match->command_string, VC_MAX_COMMAND_LEN );
        if ( modify_buffer )
        {
            //Write command completion into buffer
            memcpy( in_out_string, match->command_string, new_len );
        }

        return new_len;
    }
    return 0;
}

void vc_handle_command( const char *command_string )
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
        const char *not_found_error = "Command not recognized.\r\nTry \"help\"\r\n";
        vs_output( not_found_error, strlen( not_found_error ) );
    }
    vc_print_context( true );
}

void vc_print_context( bool active )
{
    vs_text_color( COLOR_YELLOW );
    if ( active )
    {
        vs_blink();
    }
    const char *context = "$>";
    vs_output( context, strlen( context ) );
    vs_reset_format();
}
