#include <stdint.h>

#include "SEGGER_RTT.h"

#include "void_shell.h"

int8_t shell_get_char( void ) { return SEGGER_RTT_GetKey(); }

//NOLINTNEXTLINE
int _putchar( char character ) { return SEGGER_RTT_PutChar( 0, character ); }

void shell_output( const char *data, size_t length )
{
	for ( size_t i = 0; i != length; ++i )
	{
		_putchar( data[i] );
	}
}

int main()
{
	vs_init( vs_shell_handles[0], &shell_get_char );
	vs_configure( vs_shell_handles[0], shell_get_char, shell_output, true );
	while ( true )
	{
		vs_run( vs_shell_handles[0] );
	}
}
