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
 * vs_utilities.c
 s
 * Created: 10/25/20
 * Author : Zachary Heylmun
 */

#include "void_shell_utilities.h"

#include "printf.h"

#include <stdint.h>
#include <string.h>

inline static void vs_output_escape_sequence( const char *sequence )
{
	const char escape_sequence[] = { 27, '[' };
	vs_output( escape_sequence, 2 );
	vs_output( sequence, strlen( sequence ) );
}

void vs_output( const char *data, size_t length )
{
	for ( size_t i = 0; i != length; ++i )
	{
		_putchar( data[i] );
	}
}

void vs_start_of_line() { vs_output_escape_sequence( "999D" ); }

void vs_erase_after_cursor() { vs_output_escape_sequence( "K" ); }

void vs_left() { vs_output_escape_sequence( "D" ); }

void vs_right() { vs_output_escape_sequence( "C" ); }

void vs_clear_text() { vs_output_escape_sequence( "2J" ); }

void vs_home() { vs_output_escape_sequence( "H" ); }

void vs_reset_format() { vs_output_escape_sequence( "0m" ); }

void vs_more_bold() { vs_output_escape_sequence( "1m" ); }

void vs_less_bold() { vs_output_escape_sequence( "2m" ); }

void vs_underline() { vs_output_escape_sequence( "4m" ); }

void vs_blink() { vs_output_escape_sequence( "5m" ); }

void vs_background_color( enum vs_color color )
{
	char           color_sequence[4];
	const unsigned color_code = (unsigned) color + 40u;
	snprintf( color_sequence, 4, "%um", color_code );
	vs_output_escape_sequence( color_sequence );
}

void vs_text_color( enum vs_color color )
{
	char           color_sequence[4];
	const unsigned color_code = (unsigned) color + 30u;
	snprintf( color_sequence, 4, "%um", color_code );
	vs_output_escape_sequence( color_sequence );
}
