#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int read_int()
{
    int x;
    int scanf_result = scanf("%i", &x);
    if (scanf_result < 1) abort();
    return x;
}

char *read_line()
{
    char buffer[100];
    int scanf_result = scanf(" %99[^\n]", buffer);
    if (scanf_result < 1) abort();
    char *result = strdup(buffer);
    if (result == 0) abort();
    return result;
}


int main()
{
    printf("How many students do you have? ");
    int n = read_int();
    if (n < 0 || 0x20000000 <= n) abort();
    char **names = malloc(n * sizeof(char **));
    if (names == 0) abort();
    for (int i = 0; ; i++)
    {
        if (i == n) {
            break;
        }
        printf("Please enter the name of student number %d: ", i + 1);
        char *name = read_line();
        printf("Adding '%s'...\n", name);
        names[i] = name;
    }
    
    for (;;)
    {
        printf("Please enter a student number: ");
        int k = read_int();
        if (k < 1 || n < k) {
            printf("Student number out of range. Terminating...\n");
            break;
        } else {
            char *name = names[k - 1];
            printf("Student number %d is called %s.\n", k, name);
        }
    }
    
    for (int i = 0; ; i++)
    {
        if (i == n) {
            break;
        }
        free(names[i]);
    }
    free(names);
    
    return 0;
}