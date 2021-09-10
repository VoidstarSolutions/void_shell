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
 * @file test_void_shell.c
 *
 * @date Created: 05/03/21
 * @author Zachary Heylmun
 *
 */

#include "void_shell.h"

#include <unity.h>

#include "void_command.h"
#include "void_shell_utilities.h"
#include <printf.h>
#include <stdlib.h>

int8_t shell_get_char( void ) { return getchar(); }

//NOLINTNEXTLINE
void _putchar( char character ) { putchar( character ); }

void shell_output( const char *data, size_t length )
{
	for ( size_t i = 0; i != length; ++i )
	{
		_putchar( data[i] );
	}
}

void vs_invalidate_history( struct vs_shell_data *shell );

void vs_buffer_wrapped( struct vs_shell_data *shell );

void vs_display_history_command( struct vs_shell_data *shell );

void vs_attempt_autocomplete( struct vs_shell_data *shell );

void vs_process_command( struct vs_shell_data *shell );

bool process_escape_sequence( struct vs_shell_data *shell, char input_char );

void process_received_char( struct vs_shell_data *shell, char input_char );

void setUp( void )
{
	vs_init();
	vs_configure( vs_shell_handles[0], &shell_get_char, &shell_output, true );
	vc_init( vs_shell_handles[0] );
}

void tearDown( void ) { system( "/bin/stty cooked" ); }

void test_vs_init( void )
{
	vs_init();
	TEST_ASSERT( true );
}

void test_vs_invalidate_history( void )
{
	vs_invalidate_history( vs_shell_handles[0] );
	TEST_ASSERT( true );
}