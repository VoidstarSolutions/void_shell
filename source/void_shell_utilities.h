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
 * \file void_shell_utilities.h
 *
 * \date Created: 10/25/20
 * \author Zachary Heylmun
 */

#ifndef VOID_SHELL_UTILITIES_H
#define VOID_SHELL_UTILITIES_H

#include <stddef.h>

#include "void_shell.h"

/** \brief Enumeration of colors supported for text and backgrounds */
enum vs_color
{
	COLOR_BLACK = 0,
	COLOR_RED,
	COLOR_GREEN,
	COLOR_YELLOW,
	COLOR_BLUE,
	COLOR_MAGENTA,
	COLOR_CYAN,
	COLOR_WHITE,
};

const char* vs_separator;

const char* vs_newline;

void vs_start_of_line( void );

void vs_erase_after_cursor( void );

void vs_left( void );

void vs_right( void );

void vs_clear_text( void );

void vs_home( void );

void vs_reset_format( void );

void vs_more_bold( void );

void vs_less_bold( void );

void vs_underline( void );

void vs_blink( void );

void vs_text_color( enum vs_color color );

void vs_background_color( enum vs_color );

#endif // vs_UTILITIES_H
