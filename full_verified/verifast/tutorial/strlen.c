int strlen(char *s)
{
    int i = 0;
    for (;; i++)
    {
        if (s[i] == 0) {
            break;
        }
    }
    return i;
}

int main()
{
    int n = strlen("Hello, world!");
    assert(n == 13);
    return 0;
}
