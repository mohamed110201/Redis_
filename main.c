#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif
#define MAX_KEYS 1024
#define MAX_LEN 256
#define MAX_ARGS 64

void str_upper(char *s)
{
    for (; *s; s++)
        *s = toupper(*s);
}

int parse_args(char *line, char *args[])
{
    int count = 0;
    int in_quotes = 0;
    char *start = NULL;
    char *out = line;
    for (char *p = line;; p++)
    {
        int at_end = (*p == '\0');
        if (*p == '"')
        {
            in_quotes = !in_quotes;
            continue;
        }
        if ((*p == ' ' && !in_quotes) || at_end)
        {
            if (start)
            {
                *out = '\0';
                args[count++] = start;
                start = NULL;
                out = p + 1;
            }
            if (at_end)
                break;
        }
        else
        {
            if (!start)
            {
                start = out;
            }
            if (out != p)
                *out = *p;
            out++;
        }
    }
    return count;
}

void handle(char *args[], int argc)
{
    char cmd[MAX_LEN];
    strncpy(cmd, args[0], MAX_LEN - 1);
    cmd[MAX_LEN - 1] = '\0';
    str_upper(cmd);

    if (strcmp(cmd, "PING") == 0)
    {
        if (argc == 1)
        {
            printf("+PONG\r\n");
        }
        else
        {
            printf("$%d\r\n%s\r\n", (int)strlen(args[1]), args[1]);
        }
    }
    else if (strcmp(cmd, "ECHO") == 0)
    {
        if (argc == 1)
        {
            return;
        }
        else
        {
            printf("$%d\r\n%s\r\n", (int)strlen(args[1]), args[1]);
        }
    }
    else
    {
        printf("-ERR unknown command '%s'\r\n", args[0]);
    }
    fflush(stdout);
}

int main(void)
{
    char line[4096];
    char *args[MAX_ARGS];
#ifdef _WIN32
    /* RESP uses literal CRLF; prevent Windows text-mode newline conversion. */
    _setmode(_fileno(stdout), _O_BINARY);
#endif
    while (fgets(line, sizeof(line), stdin))
    {
        line[strcspn(line, "\r\n")] = '\0';
        if (!line[0])
            continue;
        int argc = parse_args(line, args);
        if (argc > 0)
            handle(args, argc);
    }
    return 0;
}
