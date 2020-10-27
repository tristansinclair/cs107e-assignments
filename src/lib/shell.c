#include "shell.h"
#include "shell_commands.h"
#include "uart.h"
#include "printf.h"
#include "keyboard.h"
#include "strings.h"
#include "malloc.h"
#include "pi.h"
#include "gpio.h"

#define LINE_LEN 80

static formatted_fn_t shell_printf;

static const command_t commands[] = {
    {"help", "<cmd> prints a list of commands or description of cmd", cmd_help},
    {"echo", "<...> echos the user input to the screen", cmd_echo},
    {"reboot", "< > reboots the Raspberry Pi back to the bootloader", cmd_reboot},
    {"peek", "<addr> prints the contents (4 bytes) of memory at address", cmd_peek}};

const size_t COMMAND_COUNT = (sizeof(commands) / sizeof(commands[0]));

// typedef struct _command_struct {
//     const char *name;
//     const char *description;
//     command_fn_t fn;
// } command_t;

static command_t *select_command(char *command_name)
{

    for (int i = 0; i < COMMAND_COUNT; i++)
    {
        if (strcmp(command_name, commands[i].name) == 0)
        {
            return (command_t *)(commands + i); // ptr to command
        }
    }
    return 0;
}

int cmd_echo(int argc, const char *argv[])
{
    for (int i = 1; i < argc; ++i)
        shell_printf("%s ", argv[i]);
    shell_printf("\n");
    return 0;
}

int cmd_help(int argc, const char *argv[])
{
    if (argc > 1)
    {
        command_t *command_requested = select_command((char *)argv[1]);
        if (command_requested == 0)
        {
            shell_printf("error: no such command `%s`.\n", argv[1]);
            return 1; //  command not found
        }

        // if command exists
        shell_printf("%s    %s\n", command_requested->name, command_requested->description);
        return 0;
    }
    else
    {
        for (int i = 0; i < COMMAND_COUNT; i++)
        {
            shell_printf("%s    %s\n", commands[i].name, commands[i].description);
        }
        return 0;
    }
}

int cmd_reboot(int argc, const char *argv[])
{
    shell_printf("Rebooting!");
    uart_putchar(EOT);
    pi_reboot();
    return 0;
}

int cmd_peek(int argc, const char *argv[])
{
    //gpio_set_output(1);
    if (argc == 1) // only peek
    {
        shell_printf("error: peek requires 1 argument [address]\n");
        return 1;
    }

    const char *endptr;
    void *address = (void *)strtonum(argv[1], &endptr);

    if ((int)address % 4 != 0)
    {
        //shell_printf("(int)address = %x\n", (int)address);
        shell_printf("error: peek address is not 4-byte aligned\n");
        return 1;
    }

    if (*endptr != 0)
    {
        shell_printf("error: peek cannot convert '%s'\n", argv[1]);
        return 1;
    }

    unsigned int value = *(unsigned int *)address;

    shell_printf("%p:   %x\n", address, value);
    return 0;
}
int cmd_poke(int argc, const char *argv[]{
    if (argc < 3)
    {
        shell_printf("error: poke requires 2 arguments [address] and [value]\n");
        return 1;
    }

    const char *endptr;
    void *address = (void *)strtonum(argv[1], &endptr);

    if (*endptr != 0)
    {
        shell_printf("error: poke cannot convert '%s'\n", argv[1]);
        return 1;
    }
    if ((int)address % 4 != 0)
    {
        //shell_printf("(int)address = %x\n", (int)address);
        shell_printf("error: peek address is not 4-byte aligned\n");
        return 1;
    }

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

        // return
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

static char *strndup(const char *src, size_t n)
{
    char *dst = malloc(n + 1); // add bit for '\0'
    memcpy(dst, src, n);
    dst[n] = 0;
    return dst;
}

/* From lab4 heapclient.c */
static bool isspace(char ch)
{
    return ch == ' ' || ch == '\t' || ch == '\n';
}
/* From lab4 heapclient.c */
static int tokenize(const char *line, char *array[], int max)
{
    int ntokens = 0;
    const char *cur = line;

    while (ntokens < max)
    {
        while (isspace(*cur))
            cur++; // skip spaces (stop non-space/null)
        if (*cur == '\0')
            break; // no more non-space chars
        const char *start = cur;
        while (*cur != '\0' && !isspace(*cur))
            cur++;                                      // advance to end (stop space/null)
        array[ntokens++] = strndup(start, cur - start); // make heap-copy, add to array
    }
    return ntokens;
}

// typedef struct _command_struct {
//     const char *name;
//     const char *description;
//     command_fn_t fn;
// } command_t;

int shell_evaluate(const char *line)
{
    char *tokens[LINE_LEN];
    int token_count = tokenize(line, tokens, LINE_LEN);

    if (token_count == 0)
        return 0;

    command_t *command = select_command((char *)tokens[0]); // command

    if (command == 0) // if command wasn't valid
    {
        shell_printf("error: no such command `%s`.\n", tokens[0]);
        return -1; // no command was ran
    }

    int result = command->fn(token_count, tokens); // call command function

    for (int i = 0; i < token_count; i++)
    {
        free((char *)tokens[i]);
    }

    return result;
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