void memcpy(char *dest, char *src, int count)
{
    for (int i = 0; ; i++)
    {
        if (i == count) {
            break;
        }
        dest[i] = src[i];
    }
}

void memcpy1(char *dest, char *src, int count) // Identical but uses array slice syntax.
{
    for (int i = 0; ; i++)
    {
        if (i == count) {
            break;
        }
        dest[i] = src[i];
    }
}

void test()
{
    char buffer1[7] = "Hello!";
    char buffer2[7];
    memcpy(buffer2, buffer1, 7);
    assert(buffer2[5] == '!');
}