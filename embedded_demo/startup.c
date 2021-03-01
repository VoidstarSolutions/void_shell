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
void Reset_Handler( void );

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
    .reset_func = Reset_Handler, /* Reset to our entry point */
};

/**
 * \brief This is the code that gets called on processor reset.
 * To initialize the device, and call the main() routine.
 */
void Reset_Handler( void )
{
    uint32_t *pSrc, *pDest;

    /* Initialize the relocate segment */
    pSrc  = &_etext;
    pDest = &_srelocate;

    if ( pSrc != pDest )
    {
        for ( ; pDest < &_erelocate; )
        {
            *pDest++ = *pSrc++;
        }
    }

    /* Clear the zero segment */
    for ( pDest = &_szero; pDest < &_ezero; )
    {
        *pDest++ = 0;
    }

    /* Set the vector table base address */
    pSrc                                       = (uint32_t *) &_sfixed;
    uint32_t *scb_vector_table_offset_register = (uint32_t *) 0xE000E008UL;
    *scb_vector_table_offset_register =
        ( (uint32_t) pSrc & ( 0xFFFFFFUL << 8 ) );

    /* Initialize the C library */
    __libc_init_array();

    /* Branch to main function */
    main();

    /* Infinite loop */
    while ( 1 )
        ;
}
