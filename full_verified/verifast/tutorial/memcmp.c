int memcmp(char *p1, char *p2, int count)
{
    int result = 0;
    for (int i = 0; ; i++)
    {
        if (i == count) {
            break;
        }
        if (p1[i] < p2[i]) {
            result = -1; break;
        }
        if (p1[i] > p2[i]) {
            result = 1; break;
        }
    }
    return result;
}