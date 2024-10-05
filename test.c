#include <io.h>

int myadd(int a, int b) {
    return a + b;
}

int main(const int argc, const char** argv)
{
    int c;
    int d;
    c = myadd(0, 1);
    d = myadd(2, 3);
    printf("Result: %d,%d ", c, d);
    return 0;
}
