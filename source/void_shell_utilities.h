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
 * void_shell_utilities.h
 *
 * Created: 10/25/20
 * Author : Zachary Heylmun
 */

#ifndef VOID_SHELL_UTILITIES_H
#define VOID_SHELL_UTILITIES_H

#include <stddef.h>

void output( const char *data, size_t length );

void void_shell_start_of_line();

void void_shell_erase_after_cursor();

void void_shell_left();

void void_shell_right();

void void_shell_clear_text();

void void_shell_home();

#endif // VOID_SHELL_UTILITIES_H
