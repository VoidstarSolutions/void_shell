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
 * void_shell.h
 *
 * Created: 06/04/20
 * Author : Zachary Heylmun
 *
 */

#ifndef VOID_SHELL_H
#define VOID_SHELL_H

#include <stdbool.h>

/**
 * \brief Function to get next input character for console
 *
 * To Be provided by console application
 * 
 * \param[out] Application should write next input character here if available
 *
 * \return Next input character available
 **/
bool void_shell_get_char( char *in_char );

/**
 * \brief Function to write character to console
 *
 * To Be provided by console application
 * 
 * \param[out] Application should write next input character here if available
 *
 * \return Character Written Successfully
 **/
bool void_shell_write_char(char out_char );

void void_shell_run(void);

#endif // VOID_SHELL_H