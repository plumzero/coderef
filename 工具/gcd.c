
#include <stdio.h>

static unsigned gcd(unsigned a, unsigned b)
{
    unsigned c;

    while (a && b)
        if (a > b) {
            c = b;
            while (a - c >= c)
                c <<= 1;
            a -= c;
        }
        else {
            c = a;
            while (b - c >= c)
                c <<= 1;
            b -= c;
        }
    return a + b;
}

int main()
{
    printf("gcd(105, 65): %u\n", gcd(105, 65));
    printf("gcd(32, 24): %u\n", gcd(32, 24));
    printf("gcd(98, 64): %u\n", gcd(98, 64));
    
    return 0;
}