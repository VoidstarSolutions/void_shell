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

/**
 * @file void_shell.h
 * @brief Module providing shell functionality
 * Created: 06/04/20
 * Author : Zachary Heylmun
 */

#ifndef VOID_SHELL_H
#define VOID_SHELL_H

#include <stdbool.h>
#include <stdint.h>

/**
 * \brief Function to get next input character for shell
 * Must be provided by console application
 * \return Negative if not available, or ASCII value from 0-127
 **/
int8_t void_shell_get_char( void );

/**
 * \brief Initialize the memories for shell
 **/
void void_shell_init( void );

/**
 * \brief Service CLI input
 **/
void void_shell_run( void );

/**
 * \brief Clear the console of text and reset to first line/column
 **/
void void_shell_clear_console( void );

#endif // VOID_SHELL_H
