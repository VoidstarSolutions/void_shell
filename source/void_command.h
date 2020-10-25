/*******************************************************************************
**
** Copyright (c) 2020 Voidstar Solutions
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
*******************************************************************************/

/*
 * void_command.h
 *
 * Created: 06/04/20
 * Author : Zachary Heylmun
 *
 */

#ifndef VOID_COMMAND_H
#define VOID_COMMAND_H

#include <stdbool.h>
#include <stdint.h>

typedef void ( *command_handler )( void );

enum void_command_arg_type
{
	VOID_COMMAND_ARG_TYPE_INT,
	VOID_COMMAND_ARG_TYPE_FLOAT,
	VOID_COMMAND_ARG_TYPE_STRING,
	VOID_COMMAND_ARG_TYPE_COUNT,
};

struct void_command_arg
{
	enum void_command_arg_type arg_type;
	const char *               description;
	bool                       mandatory;
};

struct void_command_description
{
	const char *                   command_string;
	command_handler                command;
	const char *                   help_string;
	const struct void_command_arg *args;
	const uint8_t                  arg_count;
};

void void_command_init();

bool void_command_register( const struct void_command_description *command );

bool void_command_complete_command( char *in_out_string, uint16_t max_len );

void void_command_handle_command( const char *command_string );

#endif // VOID_COMMAND_H