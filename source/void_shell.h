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
#include <stddef.h>
#include <stdint.h>

#include "void_shell_internal.h"

typedef struct vs_data *      vs_handle;
typedef const struct vs_data *const_vs_handle;

/** 
 * @brief vs_shell_data will be an array of pointers to shell
 * data of size VS_SHELL_COUNT
 **/
extern vs_handle vs_handles[];

/**
 * @brief Initialize the memories for shell
 **/
void vs_init();

/**
 * @brief Configure a shell with the provided input function
 * 
 * @param [in,out] shell The shell to configure
 * @param [in] input_func int8_t ( *vs_get_char )() function pointer to 
 * retrieve next input character for shell
 * @param [in] output_func 
 * void ( *vs_output )( const char *data, size_t length ) function pointer
 * to handle shell output
 * @param [in] echo_enabled Whether the shell should echo typed characters,
 * newlines, and escape sequences
 **/
void vs_configure( vs_handle   shell,
                   vs_get_char input_func,
                   vs_output   output_func,
                   bool        echo_enabled );

/**
 * @brief Service CLI input
 * 
 * @param [in,out] shell Shell to service
 **/
void vs_run( vs_handle shell );

/**
 * @brief Clear the console of text and reset to first line/column
 * 
 * @param [in,out] shell Shell to clear console
 **/
void vs_clear_console( vs_handle shell );

/**
 * @brief internal function to handle output from shell
 * Respects shell's echo settings.  If echo is disabled, the output function is
 * not called.
 * 
 * @param [in] shell Shell for output
 * @param [in] data characters to print
 * @param [in] length number of characters to output
 **/
void vs_output_internal( const_vs_handle shell, const char *data, size_t length );

#endif // vs_H
