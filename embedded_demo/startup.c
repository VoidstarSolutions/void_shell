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

void _start( void );

void __libc_init_array( void );

typedef struct
{
	void *stack;
	void ( *reset_func )();
} reset_vector_t;

/* Exception Table */
__attribute__( ( section( ".vectors" ) ) ) const reset_vector_t exception_table = {

    /* Configure Initial Stack Pointer, using linker-generated symbols */
    .stack      = (void *) ( &_estack ),
    .reset_func = _start, /* Reset to our entry point */
};
