#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ECC_MAX_XCOORDINATE_BITS_LEN    512

//长度为176 (0xb0)
typedef struct Struct_ECCCIPHERBLOB1 {
    unsigned char XCoordinate[ECC_MAX_XCOORDINATE_BITS_LEN/8];      //64
    unsigned char YCoordinate[ECC_MAX_XCOORDINATE_BITS_LEN/8];      //64
    unsigned char HASH[32];             //32
    unsigned long CipherLen;            //8
    unsigned char Cipher[1];
} ECCCIPHERBLOB1;
//长度为168 (0xa8)
typedef struct Struct_ECCCIPHERBLOB3 {
    unsigned char XCoordinate[ECC_MAX_XCOORDINATE_BITS_LEN/8];
    unsigned char YCoordinate[ECC_MAX_XCOORDINATE_BITS_LEN/8];
    unsigned char HASH[32];
    unsigned long CipherLen;
    unsigned char Cipher[];
} ECCCIPHERBLOB2;

int main()
{
    ECCCIPHERBLOB1 *blob1 = NULL, *blob2 = NULL; 

    size_t len1 = 0, len2 = 0;
    
    int i = 0;
    
    unsigned char data[8] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};
    
    len1 = sizeof(ECCCIPHERBLOB1) + sizeof(data);   //len = 0xb8
    len2 = sizeof(ECCCIPHERBLOB2) + sizeof(data);   //len = 0xb0
    
    blob1 = (ECCCIPHERBLOB1*)calloc(1, len1);
    if (!blob1)
    {
        fprintf(stderr, "%d error\n", __LINE__);
        return -1;
    }
    else
    {
        //假设从0开始分配地址，blob1占用空间[0x0, 0xb8)，blob1->Cipher的地址为0xa8，[0xa8, 0xb0)被data占用，[0xb0, 0xb8)被白白浪费
        memcpy(blob1->Cipher, data, 8);
        printf("len(%lu) %p %p\n", len1, blob1, blob1->Cipher);
        for (i = 0; i < 8; i++)
            printf("%c %p\n", blob1->Cipher[i], &(blob1->Cipher[i]));
        printf("\n");
    }
    
    blob2 = (ECCCIPHERBLOB1*)calloc(1, len2);
    if (!blob2)
    {
        fprintf(stderr, "%d error\n", __LINE__);
        return -1;
    }
    else
    {
        //假设从0开始分配地址，blob2占用空间[0x0, 0xb0)，blob2->Cipher的地址为0xa8，[0xa8, 0xb0)被data占用，没有浪费内存
        memcpy(blob2->Cipher, data, 8);
        printf("len(%lu) %p %p\n", len2, blob2, blob2->Cipher);
        for (i = 0; i < 8; i++)
            printf("%c %p\n", blob2->Cipher[i], &(blob2->Cipher[i]));
        printf("\n");
    }
    
    return 0;
}

