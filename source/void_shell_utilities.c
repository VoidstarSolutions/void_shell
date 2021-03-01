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
 * void_shell_utilities.c
 s
 * Created: 10/25/20
 * Author : Zachary Heylmun
 */

#include "void_shell_utilities.h"

#include <../third_party/printf/printf.h>

#include <string.h>

inline static void output_escape_sequence( const char *sequence )
{

	const char escape_sequence[] = { 27, '[' };
	output( escape_sequence, 2 );
	output( sequence, strlen( sequence ) );
}

void output( const char *data, size_t length )
{
	for ( size_t i = 0; i != length; ++i )
	{
		_putchar( data[i] );
	}
}

void void_shell_start_of_line() { output_escape_sequence( "999D" ); }

void void_shell_erase_after_cursor() { output_escape_sequence( "K" ); }

void void_shell_left() { output_escape_sequence( "D" ); }

void void_shell_right() { output_escape_sequence( "C" ); }

void void_shell_clear_text() { output_escape_sequence( "2J" ); }

void void_shell_home() { output_escape_sequence( "H" ); }
