char getchar();
void putchar(char c);

char *malloc(int count);

void getchars(char *start, int count)
{
    int i = 0;
    while (i < count)
    {
        char c = getchar();
        *(start + i) = c;
        i++;
    }
}

void putchars(char *start, int count)
{
    int i = 0;
    while (i < count)
    {
        putchar(*(start + i));
        i++;
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
