
#include <stdio.h>

int main()
{
    {
        int i = 2, j = 3;
        
        double k = (i * j) / 8 ? (i * j) / 8 : 10;
        
        printf("k=%f\n", k);        
    }

    {
        int i = 2, j = 3;
        
        double k = (i * j) / 4 ? (i * j) / 4 : 10;
        
        printf("k=%f\n", k);        
    }
    
    
    return 0;
}