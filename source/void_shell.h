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
 * \file void_shell.h
 * \brief Module providing shell functionality
 * \date created: 06/04/20
 * \author Zachary Heylmun
 */

#ifndef VOID_SHELL_H
#define VOID_SHELL_H

#include <stdbool.h>
#include <stdint.h>

struct vs_shell_data;

typedef struct vs_shell_data * vs_shell_data_t;

extern vs_shell_data_t vs_static_shell;

/**
 * \brief Function to get next input character for shell
 * Must be provided by console application
 * \return Negative if not available, or ASCII value from 0-127
 **/
typedef int8_t (*vs_get_char)();

/**
 * \brief Initialize the memories for shell
 **/
void vs_init( vs_shell_data_t shell, vs_get_char input_func );

/**
 * \brief Enable or disable shell echo
 * \param [in] echo_enabled Whether the shell should echo recieved characters
 **/
void vs_set_echo_enabled( vs_shell_data_t shell, bool echo_enabled ); 

/**
 * \brief Service CLI input
 **/
void vs_run( vs_shell_data_t shell );

/**
 * \brief Clear the console of text and reset to first line/column
 **/
void vs_clear_console( vs_shell_data_t shell );

#endif // vs_H
