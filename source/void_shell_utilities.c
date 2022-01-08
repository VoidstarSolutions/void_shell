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
 * \file void_shell_utilities.c
 *
 * \date created: 10/25/20
 * \author Zachary Heylmun
 */

#include "void_shell_utilities.h"

#include <stdint.h>
#include <string.h>

const char *vs_separator = ": ";

const char *vs_newline = "\r\n";

void vs_start_of_line( void )
{
    const char escape_sequence[] = { 27, '[', '9', '9', '9', 'D', '\0' };
    vs_output( escape_sequence, sizeof( escape_sequence ) );
}

void vs_erase_after_cursor( void )
{
    const char escape_sequence[] = { 27, '[', 'K', '\0' };
    vs_output( escape_sequence, sizeof( escape_sequence ) );
}

void vs_left( void )
{
    const char escape_sequence[] = { 27, '[', 'D', '\0' };
    vs_output( escape_sequence, sizeof( escape_sequence ) );
}

void vs_right( void )
{
    const char escape_sequence[] = { 27, '[', 'C', '\0' };
    vs_output( escape_sequence, sizeof( escape_sequence ) );
}

void vs_clear_text( void )
{
    const char escape_sequence[] = { 27, '[', '2', 'J', '\0' };
    vs_output( escape_sequence, sizeof( escape_sequence ) );
}

void vs_home( void )
{
    const char escape_sequence[] = { 27, '[', 'H', '\0' };
    vs_output( escape_sequence, sizeof( escape_sequence ) );
}

void vs_reset_format( void )
{
    const char escape_sequence[] = { 27, '[', '0', 'm', '\0' };
    vs_output( escape_sequence, sizeof( escape_sequence ) );
}

void vs_more_bold( void )
{
    const char escape_sequence[] = { 27, '[', '1', 'm', '\0' };
    vs_output( escape_sequence, sizeof( escape_sequence ) );
}

void vs_less_bold( void )
{
    const char escape_sequence[] = { 27, '[', '2', 'm', '\0' };
    vs_output( escape_sequence, sizeof( escape_sequence ) );
}

void vs_underline( void )
{
    const char escape_sequence[] = { 27, '[', '4', 'm', '\0' };
    vs_output( escape_sequence, sizeof( escape_sequence ) );
}

void vs_blink( void )
{
    const char escape_sequence[] = { 27, '[', '5', 'm', '\0' };
    vs_output( escape_sequence, sizeof( escape_sequence ) );
}

void vs_background_color( enum vs_color color )
{
    const uint8_t color_value      = (uint8_t) color + 48u;
    const char    color_sequence[] = { 27, '[', '3', color_value, 'm', '\0' };
    vs_output( color_sequence, sizeof( color_sequence ) );
}

void vs_text_color( enum vs_color color )
{
    const uint8_t color_value      = (uint8_t) color + 48u;
    const char    color_sequence[] = { '\e', '[', '3', color_value, 'm', '\0' };
    vs_output( color_sequence, sizeof( color_sequence ) );
}
