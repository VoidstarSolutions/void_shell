#include <stdbool.h>
#include <stdint.h>

/* Initialize segments */
extern uint32_t _sfixed;
extern uint32_t _efixed;
extern uint32_t _etext;
extern uint32_t _srelocate;
extern uint32_t _erelocate;
extern uint32_t _szero;
extern uint32_t _ezero;
extern uint32_t _sstack;
extern uint32_t _estack;

int  main( void );
void reset_handler( void );

void __libc_init_array( void );

typedef struct
{
	void *stack;
	void ( *reset_func )();
} reset_vector_t;

/**
 * \brief Exception Table 
 * Only defines stack pointer and reset handler.
 * All other interrupts are invalid
 **/
__attribute__( ( section( ".vectors" ), used ) ) const reset_vector_t exception_table = {
    /* Configure Initial Stack Pointer, using linker-generated symbols */
    .stack      = (void *) ( &_estack ),
    .reset_func = &reset_handler, /* Reset to our entry point */
};

/**
 * \brief This is the code that gets called on processor reset.
 * to initialize the device, and call the main() routine.
 */
__attribute__( ( noreturn ) ) void reset_handler( void )
{
	uint32_t *src  = &_etext;
	uint32_t *dest = &_srelocate;

	if ( src != dest )
	{
		while ( dest < &_erelocate )
		{
			*dest++ = *src++;
		}
	}

	dest = &_szero;
	/* Clear the zero segment */
	while ( dest < &_ezero )
	{
		*dest++ = 0;
	}

	/* Set the vector table base address */
	src                                        = &_sfixed;
	uint32_t *scb_vector_table_offset_register = (uint32_t *) 0xE000E008UL;
	*scb_vector_table_offset_register          = ( (uint32_t) src & ( 0xFFFFFFUL << 8 ) );

	/* Initialize the C library */
	__libc_init_array();

	/* Branch to main function */
	main();

	
	while ( true )
	{
        /* Infinite loop */
	}
}
