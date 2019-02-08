char getchar();
void putchar(char c);

char *malloc(int count);

void getchars(char *start, int count)
{
    for (int i = 0; i < count; i++)
    {
        char c = getchar();
        *(start + i) = c;
    }
}

void putchars(char *start, int count)
{
    for (int i = 0; i < count; i++)
    {
        char c = *(start + i);
        putchar(c);
    }
}

int main()
{
    char *array = malloc(100);
    getchars(array, 100);
    putchars(array, 100);
    putchars(array, 100);
    return 0;
}
