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

#include "printf.h"

#include <stdint.h>
#include <string.h>

void vs_output( const char *data, size_t length )
{
	for ( size_t i = 0; i != length; ++i )
	{
		_putchar( data[i] );
	}
}

void vs_start_of_line()
{
	const char escape_sequence[] = { 27, '[', '9', '9', '9', 'D' };
	vs_output( escape_sequence, sizeof( escape_sequence ) );
}

void vs_erase_after_cursor()
{
	const char escape_sequence[] = { 27, '[', 'K' };
	vs_output( escape_sequence, sizeof( escape_sequence ) );
}

void vs_left()
{
	const char escape_sequence[] = { 27, '[', 'D' };
	vs_output( escape_sequence, sizeof( escape_sequence ) );
}

void vs_right()
{
	const char escape_sequence[] = { 27, '[', 'C' };
	vs_output( escape_sequence, sizeof( escape_sequence ) );
}

void vs_clear_text()
{
	const char escape_sequence[] = { 27, '[', '2', 'J' };
	vs_output( escape_sequence, sizeof( escape_sequence ) );
}

void vs_home()
{
	const char escape_sequence[] = { 27, '[', 'H' };
	vs_output( escape_sequence, sizeof( escape_sequence ) );
}

void vs_reset_format()
{
	const char escape_sequence[] = { 27, '[', '0', 'm' };
	vs_output( escape_sequence, sizeof( escape_sequence ) );
}

void vs_more_bold()
{
	const char escape_sequence[] = { 27, '[', '1', 'm' };
	vs_output( escape_sequence, sizeof( escape_sequence ) );
}

void vs_less_bold()
{
	const char escape_sequence[] = { 27, '[', '2', 'm' };
	vs_output( escape_sequence, sizeof( escape_sequence ) );
}

void vs_underline()
{
	const char escape_sequence[] = { 27, '[', '4', 'm' };
	vs_output( escape_sequence, sizeof( escape_sequence ) );
}

void vs_blink()
{
	const char escape_sequence[] = { 27, '[', '5', 'm' };
	vs_output( escape_sequence, sizeof( escape_sequence ) );
}

void vs_background_color( enum vs_color color )
{
	char           color_sequence[6] = { 27 };
	const unsigned color_code        = (unsigned) color + 40u;
	snprintf( &color_sequence[1], 5, "[%um", color_code );
	vs_output( color_sequence, sizeof( color_sequence ) );
}

void vs_text_color( enum vs_color color )
{
	char           color_sequence[6] = { 27 };
	const unsigned color_code        = (unsigned) color + 30u;
	snprintf( &color_sequence[1], 5, "[%um", color_code );
	vs_output( color_sequence, sizeof( color_sequence ) );
}
