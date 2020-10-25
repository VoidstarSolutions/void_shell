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

#include "void_command.h"

#include <string.h>

#include <printf.h>

#include "void_shell.h"

#ifndef VOID_COMMAND_MAX_COMMANDS
#define VOID_COMMAND_MAX_COMMANDS ((uint16_t)32)
#endif // VOID_COMMAND_MAX_COMMANDS

struct void_command_data
{
	uint16_t registered_command_count;
	struct void_command_description const *registered_commands[VOID_COMMAND_MAX_COMMANDS];
};

static struct void_command_data void_command_instance;

static void help_command(void)
{
	struct void_command_data *command = &void_command_instance;
	for (uint16_t i = 1; i != command->registered_command_count; ++i)
	{
		const struct void_command_description *desc = command->registered_commands[i];
		printf("%s", desc->command_string);
		printf(":\n	");
		printf("%s", desc->help_string);
		for (uint8_t j = 0; j != desc->arg_count; ++j)
		{
			printf(":\n		");
			printf("%s", desc->args[j].description);
		}
		printf("\n");
	}
}

static const struct void_command_description help_command_description = {
	"help",
	help_command,
	"The following are the currently registered commands:",
	0,
	0,
};

static void clear_command(void) { void_shell_clear(); }

static const struct void_command_description clear_command_description = {
	"clear",
	clear_command,
	"Clear the terminal",
	0,
	0,
};

void void_command_init()
{
	struct void_command_data *command = &void_command_instance;
	command->registered_command_count = 0;
	void_command_register(&help_command_description);
	void_command_register(&clear_command_description);
}

bool void_command_register(const struct void_command_description *description)
{
	struct void_command_data *command = &void_command_instance;
	if (command->registered_command_count == VOID_COMMAND_MAX_COMMANDS)
	{
		printf("Max Commands reached.  Please increase VOID_COMMAND_MAX_COMMANDS\n");
		return false;
	}
	command->registered_commands[command->registered_command_count++] = description;
	return true;
}

bool void_command_complete_command(char *in_out_string, uint16_t max_len)
{
	(void)(max_len);
	// struct void_command_data *command = &void_command_instance;
	size_t input_len = strlen(in_out_string);
	// for(uint16_t command_index = 0; command_index != )
	for (size_t i = 0; i != input_len; ++i)
	{
	}
	return false;
}

void void_command_handle_command(const char *command_string)
{
	struct void_command_data *command = &void_command_instance;
	for (uint16_t command_index = 0; command_index != command->registered_command_count;
		 ++command_index)
	{
		if (strcmp(command->registered_commands[command_index]->command_string,
				   command_string) == 0)
		{
			return command->registered_commands[command_index]->command();
		}
	}

	printf("Command not recognized.\n	Try \"help\"\n");
}