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

#ifndef VS_SHELL_COUNT
/** Default is a single shell */
#define VS_SHELL_COUNT ( (uint8_t) 1 )
#endif // VS_SHELL_COUNT

#ifndef VS_BUFFER_SIZE_POW_TWO
/** Default buffer size of 2^8, or 256 characters */
#define VS_BUFFER_SIZE_POW_TWO ( (uint8_t) 8 )
#endif // VS_BUFFER_SIZE_POW_TWO

#ifndef VS_COMMAND_HISTORY_COUNT_POW_TWO
/** Default history length of 2^2, or 4 commands */
#define VS_COMMAND_HISTORY_COUNT_POW_TWO ( (uint8_t) 2 )
#endif // VS_COMMAND_HISTORY_COUNT_POW_TWO

struct vs_shell_data;

typedef struct vs_shell_data *vs_shell_handle;

/** 
 * @brief vs_shell_data will be an array of pointers to shell
 * data of size VS_SHELL_COUNT
 **/
extern vs_shell_handle vs_shell_handles[];

/**
 * @brief Function to get next input character for shell
 * Must be provided by console application
 * @return Negative if not available, or ASCII value from 0-127
 **/
typedef int8_t ( *vs_get_char )();

typedef void (*vs_output)( const char *data, size_t length );

/**
 * @brief Initialize the memories for shell
 **/
void vs_init();

/**
 * @brief Configure a shell with the provided input function
 * 
 * @param [in,out] shell The shell to configure
 * @param [in] input_func vs_get_char function pointer to retrieve
 * next input character for shell
 * @param [in] echo_enabled Whether the shell should echo typed characters,
 * newlines, and escape sequences
 **/
void vs_configure( vs_shell_handle shell, vs_get_char input_func, vs_output output_func, bool echo_enabled );

/**
 * @brief Service CLI input
 * 
 * @param [in,out] shell Shell to service
 **/
void vs_run( vs_shell_handle shell );

/**
 * @brief Clear the console of text and reset to first line/column
 * 
 * @param [in,out] shell Shell to clear console
 **/
void vs_clear_console( vs_shell_handle shell );

/**
 * @brief internal function to handle output from shell
 * DO NOT CALL
 * 
 * @param [in] shell Shell for output
 * @param [in] data characters to print
 * @param [in] length number of characters to output
 **/
void vs_output_internal( vs_shell_handle shell, const char *data, size_t length );

#endif // vs_H
