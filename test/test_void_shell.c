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

#include <stdlib.h>
#include <string.h>

#include "../third_party/printf/printf.h"
#include "../third_party/unity/src/unity.h"

#include "../build/test/mocks/mock_void_command.h"
#include "../build/test/mocks/mock_void_shell_utilities.h"

// Extern the shell data
extern struct vs_data vs_data[];

static uint8_t     input_index  = 0;
static uint8_t     output_index = 0;
static char        output_buffer[256];
static const char  sequential_comparison_array[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
static const char *comparison_ptr;

int8_t shell_get_char( void ) { return comparison_ptr[input_index++]; }

static void reset_output_buffer()
{
	memset( &output_buffer[0], 0, 256 );
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

void test_vs_invalidate_history( void )
{
	struct vs_data *shell = &vs_data[0];

	shell->previous_commands[0] = ( struct vs_history_entry ){ 0, 10 };
	shell->previous_commands[1] = ( struct vs_history_entry ){ 10, 10 };
	shell->previous_commands[2] = ( struct vs_history_entry ){ 20, 10 };
	shell->previous_commands[3] = ( struct vs_history_entry ){ 30, 10 };
	
	shell->line_length = 0;
	shell->start_index = 1;
	// If line has no characters, don't invalidate anything
	vs_invalidate_history( vs_handles[0] );
	TEST_ASSERT_EQUAL( 10, shell->previous_commands[0].length );
	TEST_ASSERT_EQUAL( 0, shell->previous_commands[0].start_index );
	shell->line_length = 1;
	// should invalidate first command, but not others
	vs_invalidate_history( vs_handles[0] );
	TEST_ASSERT_EQUAL( 10, shell->previous_commands[1].length );
	TEST_ASSERT_EQUAL( 10, shell->previous_commands[1].start_index );
	TEST_ASSERT_EQUAL( 10, shell->previous_commands[2].length );
	TEST_ASSERT_EQUAL( 20, shell->previous_commands[2].start_index );
	TEST_ASSERT_EQUAL( 10, shell->previous_commands[3].length );
	TEST_ASSERT_EQUAL( 30, shell->previous_commands[3].start_index );

	// invalidate third but not second or last
	shell->start_index = 21;
	vs_invalidate_history( vs_handles[0] );
	TEST_ASSERT_EQUAL( 10, shell->previous_commands[1].length );
	TEST_ASSERT_EQUAL( 10, shell->previous_commands[1].start_index );
	TEST_ASSERT_EQUAL( 0, shell->previous_commands[2].length );
	TEST_ASSERT_EQUAL( 0, shell->previous_commands[2].start_index );
	TEST_ASSERT_EQUAL( 10, shell->previous_commands[3].length );
	TEST_ASSERT_EQUAL( 30, shell->previous_commands[3].start_index );

}

void test_vs_buffer_wrapped()
{
	comparison_ptr        = &sequential_comparison_array[0];
	input_index           = 0;
	struct vs_data *shell = &vs_data[0];

	// write some data into the end of the buffer
	size_t start_index = VS_BUFFER_SIZE - 10;
	shell->start_index = start_index;
	size_t counter       = 0;
	for ( size_t buffer_index = start_index; buffer_index < VS_BUFFER_SIZE; ++buffer_index )
	{
		shell->input_buffer[buffer_index] = counter++;
	}

	shell->line_length = counter;
	vs_buffer_wrapped( shell );

	TEST_ASSERT_EACH_EQUAL_CHAR( 0, &shell->input_buffer[start_index], 9 );
	TEST_ASSERT_EQUAL( 0, shell->input_buffer[0] );
	TEST_ASSERT_EQUAL_CHAR_ARRAY( comparison_ptr, &shell->input_buffer[0], 10 );
}

void test_vs_display_history_command()
{
	comparison_ptr = &sequential_comparison_array[0];
	input_index    = 0;
	reset_output_buffer();

	struct vs_data *shell = &vs_data[0];

	shell->previous_commands[0] = ( struct vs_history_entry ){ 0, 3 };
	shell->previous_commands[1] = ( struct vs_history_entry ){ 3, 3 };
	shell->previous_commands[2] = ( struct vs_history_entry ){ 6, 3 };
	shell->previous_commands[3] = ( struct vs_history_entry ){ 9, 1 };

	memcpy( &shell->input_buffer, comparison_ptr, 10 );

	vs_start_of_line_Expect( shell );
	vs_erase_after_cursor_Expect( shell );
	vc_print_context_Expect();
	vs_display_history_command( shell );
	TEST_ASSERT_EQUAL_CHAR_ARRAY( comparison_ptr, &output_buffer[0], 3 );
	shell->dirty                   = true;
	shell->command_index           = 3;
	shell->requested_command_index = 2;
	shell->line_length             = 3;
	shell->start_index             = 6;
	memcpy( &shell->input_buffer[0], comparison_ptr, 3 );
	vs_start_of_line_Expect( shell );
	vs_erase_after_cursor_Expect( shell );
	vc_print_context_Expect();
	vs_display_history_command( shell );
	// Should have printed 6-8
	TEST_ASSERT_EQUAL_CHAR_ARRAY( &comparison_ptr[6], &output_buffer[3], 3 );
	// Shell was dirty, so should have overwritten command 3(current) history
	TEST_ASSERT_EQUAL( 6, shell->previous_commands[3].start_index );
	TEST_ASSERT_EQUAL( 3, shell->previous_commands[3].length );
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
	reset_output_buffer();
	const char *test_out = "A,B,C,D";
	// Test with echo enabled
	vs_output_internal( vs_handles[0], test_out, strlen( test_out ) );
	TEST_ASSERT_EQUAL_CHAR( 'A', output_buffer[0] );
	TEST_ASSERT_EQUAL_CHAR( 'D', output_buffer[6] );
	TEST_ASSERT_EQUAL_CHAR( '\0', output_buffer[7] );
	vs_configure( vs_handles[0], &shell_get_char, &shell_output, false );
	reset_output_buffer();
	// Test with echo disabled
	vs_output_internal( vs_handles[0], test_out, strlen( test_out ) );
	TEST_ASSERT_EQUAL_CHAR( '\0', output_buffer[0] );
	TEST_ASSERT_EQUAL_CHAR( '\0', output_buffer[6] );
}
