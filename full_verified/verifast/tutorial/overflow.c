#include "stdlib.h"

int int_add(int x, int y)
{
    if (0 <= x) {
        if (INT_MAX - x < y) abort();
    } else {
        if (y < INT_MIN - x) abort();
    }
    return x + y;
}

int main()
{
    int x = int_add(2000000000, 2000000000);
    assert(0 <= x);
    return 0;
}
