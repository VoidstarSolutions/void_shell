#include <stdint.h>

#include "../third_party/printf/printf.h"
#include "../third_party/RTT/SEGGER_RTT.h"

#include "void_command.h"
#include "void_shell.h"
#include "void_shell_utilities.h"


int8_t shell_get_char( void ) { return (int8_t) SEGGER_RTT_GetKey(); }

//NOLINTNEXTLINE
void _putchar( char character ) { SEGGER_RTT_PutChar( 0, character ); }

void shell_output( const char *data, size_t length )
{
	for ( size_t i = 0; i != length; ++i )
	{
		_putchar( data[i] );
	}
}

void vc_print_greeting( const_vs_handle shell )
{
	vs_text_color( shell, COLOR_GREEN );
	vs_more_bold( shell );
	vs_more_bold( shell );
	printf( "  _    _  _____  _____ ______       _______ _     _ _______               \r\n" );
	printf( "   \\  /  |     |   |   |     \\      |______ |_____| |______ |      |      \r\n" );
	printf( "    \\/   |_____| __|__ |_____/      ______| |     | |______ |_____ |_____ \r\n\r\n" );

	vs_reset_format( shell );
}

int main()
{ 
	vs_init(vs_default_shell);
	vs_configure( vs_default_shell, &shell_get_char, &shell_output, true );
	vc_init();
	vc_activate( vs_default_shell );
	while ( true )
	{
		vs_run( vs_default_shell );
	}
}
