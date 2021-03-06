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

/* Exception Table */
__attribute__( ( section( ".vectors" ) ) )
const reset_vector_t exception_table = {
    /* Configure Initial Stack Pointer, using linker-generated symbols */
    .stack      = (void *) ( &_estack ),
    .reset_func = reset_handler, /* Reset to our entry point */
};

/**
 * \brief This is the code that gets called on processor reset.
 * To initialize the device, and call the main() routine.
 */
void reset_handler( void )
{
    uint32_t *src, *dest;

    /* Initialize the relocate segment */
    src  = &_etext;
    dest = &_srelocate;

    if ( src != dest )
    {
        for ( ; dest < &_erelocate; )
        {
            *dest++ = *src++;
        }
    }

    /* Clear the zero segment */
    for ( dest = &_szero; dest < &_ezero; )
    {
        *dest++ = 0;
    }

    /* Set the vector table base address */
    src                                       = (uint32_t *) &_sfixed;
    uint32_t *scb_vector_table_offset_register = (uint32_t *) 0xE000E008UL;
    *scb_vector_table_offset_register =
        ( (uint32_t) src & ( 0xFFFFFFUL << 8 ) );

    /* Initialize the C library */
    __libc_init_array();

    /* Branch to main function */
    main();

    /* Infinite loop */
    while ( 1 )
        ;
}
