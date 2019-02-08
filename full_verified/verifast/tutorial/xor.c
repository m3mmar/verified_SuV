char getchar();
void putchar(char c);

char *malloc(int count);

void getchars(char *start, int count)
{
    if (count > 0) {
        char c = getchar();
        *start = c;
        getchars(start + 1, count - 1);
    }
}

void xorchars(char *text, char *key, int count)
{
    if (count > 0) {
        *text = (char)(*text ^ *key);
        xorchars(text + 1, key + 1, count - 1);
    }
}

void putchars(char *start, int count)
{
    if (count > 0) {
        char c = *start;
        putchar(c);
        putchars(start + 1, count - 1);
    }
}

int main()
{
    char *text = malloc(10);
    char *key = malloc(10);
    getchars(text, 10);
    getchars(key, 10);
    xorchars(text, key, 10);
    putchars(text, 10);
    return 0;
}
