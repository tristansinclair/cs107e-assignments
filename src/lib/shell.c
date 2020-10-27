#include "shell.h"
#include "shell_commands.h"
#include "uart.h"
#include "printf.h"
#include "keyboard.h"

#define LINE_LEN 80

static formatted_fn_t shell_printf;

static const command_t commands[] = {
    {"help", "<cmd> prints a list of commands or description of cmd", cmd_help},
    {"echo", "<...> echos the user input to the screen", cmd_echo},
};

int cmd_echo(int argc, const char *argv[])
{
    for (int i = 1; i < argc; ++i)
        shell_printf("%s ", argv[i]);
    shell_printf("\n");
    return 0;
}

int cmd_help(int argc, const char *argv[])
{
    // TODO: your code here
    return 0;
}

void shell_init(formatted_fn_t print_fn)
{
    shell_printf = print_fn;
}

void shell_bell(void)
{
    uart_putchar('\a');
}

void shell_readline(char buf[], size_t bufsize)
{
    // printf("Size of buf: %d", bufsize); 80

    int counter = 0;
    unsigned char current = 0;

    while (1)
    {
        current = keyboard_read_next();

        // handle backspace
        if (current == '\b')
        {
            if (counter > 0)
            {
                buf[counter] = 0; // w/ extension needs to be fixed up
                shell_printf("%c", '\b');
                shell_printf("%c", ' ');
                shell_printf("%c", '\b');
                counter--;
                continue;
            }
            if (counter == 0)
            {
                continue;
            }
        }

        // check counter and bufsize
        if (counter == bufsize - 1) 
        {
            continue;
        }

        if (current == '\n')
        {
            buf[counter] = 0;
            break;
        }

        // need to fix bad chars
        buf[counter] = current;
        shell_printf("%c", current);
        counter++;
    }

    // after break
    shell_printf("%c", '\n');
}

int shell_evaluate(const char *line)
{
    shell_printf("%s", line);

    return 1;
}

void shell_run(void)
{
    shell_printf("Welcome to the CS107E shell. Remember to type on your PS/2 keyboard!\n");
    while (1)
    {
        char line[LINE_LEN];

        shell_printf("Pi> ");
        shell_readline(line, sizeof(line));
        shell_evaluate(line);
    }
}