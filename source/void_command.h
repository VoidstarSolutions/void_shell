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
 * \file void_command.h
 *
 * \date Created: 06/04/20
 * \author Zachary Heylmun
 *
 */

#ifndef VOID_COMMAND_H
#define VOID_COMMAND_H

#include <stdbool.h>
#include <stddef.h>

#include "void_shell.h"

typedef void ( *command_handler )( void );

/**
 * Struct to define a new command. Defines a single command processor
 */
struct void_command_description
{
	/** console command string */
	const char *command_string;
	/** function called by command  */
	const command_handler command;
	/** Help string for command */
	const char *help_string;
};

void vc_init( vs_shell_handle shell );

bool vc_register( const struct void_command_description *command );

size_t vc_complete_command( char *in_out_string, size_t max_len );

void vc_handle_command( const char *command_string );

void vc_print_context( void );

#endif // VOID_COMMAND_H
