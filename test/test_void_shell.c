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

#include <printf.h>
#include <stdlib.h>
#include <string.h>

#include <unity.h>

#include "mock_void_command.h"
#include "mock_void_shell_utilities.h"

// Extern the shell data
extern struct vs_data vs_data[];

int8_t shell_get_char( void ) { return getchar(); }

static uint8_t output_index = 0;
static char    output_buffer[256];

static void reset_buffer()
{
	for ( uint8_t index = 0; index < output_index; ++index )
	{
		output_buffer[index] = '\0';
	}
	output_index = 0;
}

//NOLINTNEXTLINE
void _putchar( char character ) { output_buffer[output_index++] = character; }

void shell_output( const char *data, size_t length )
{
	for ( size_t i = 0; i != length; ++i )
	{
		_putchar( data[i] );
	}
}

void vs_invalidate_history( struct vs_data *shell );

void vs_buffer_wrapped( struct vs_data *shell );

void vs_display_history_command( struct vs_data *shell );

void vs_attempt_autocomplete( struct vs_data *shell );

void vs_process_command( struct vs_data *shell );

bool process_escape_sequence( struct vs_data *shell, char input_char );

void process_received_char( struct vs_data *shell, char input_char );

static void configure_shell( bool with_echo )
{
	vs_clear_text_Expect( &vs_data[0] );
	vs_home_Expect( &vs_data[0] );
	vs_init();
	vs_configure( vs_handles[0], &shell_get_char, &shell_output, with_echo );
}

void setUp( void ) { configure_shell( true ); }

void tearDown( void ) { system( "/bin/stty cooked" ); }

void test_vs_invalidate_history( void )
{
	vs_invalidate_history( vs_handles[0] );
	TEST_ASSERT( true );
}

void test_vs_init( void )
{
	vs_clear_text_Expect( vs_handles[0] );
	vs_home_Expect( &vs_data[0] );
	vs_init();
	TEST_ASSERT_EQUAL_PTR( &vs_data[0], vs_handles[0] );
	const uint8_t *shell_data = (uint8_t *) &vs_data[0];
	TEST_ASSERT_EQUAL_UINT8_ARRAY( 0, *shell_data, sizeof( struct vs_data ) );
}

void test_vs_output_internal( void )
{
	vs_configure( vs_handles[0], &shell_get_char, &shell_output, true );
	reset_buffer();
	const char *test_out = "A,B,C,D";
	// Test with echo enabled
	vs_output_internal( vs_handles[0], test_out, strlen( test_out ) );
	TEST_ASSERT_EQUAL_CHAR( 'A', output_buffer[0] );
	TEST_ASSERT_EQUAL_CHAR( 'D', output_buffer[6] );
	TEST_ASSERT_EQUAL_CHAR( '\0', output_buffer[7] );
	vs_configure( vs_handles[0], &shell_get_char, &shell_output, false );
	reset_buffer();
	// Test with echo disabled
	vs_output_internal( vs_handles[0], test_out, strlen( test_out ) );
	TEST_ASSERT_EQUAL_CHAR( '\0', output_buffer[0] );
	TEST_ASSERT_EQUAL_CHAR( '\0', output_buffer[6] );
}
