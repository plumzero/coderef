
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>

static unsigned char ucs[32] = {
    0x20, 0x20, 0x20, 0x20, 0x2B, 0x20, 0xE6, 0x9C,
    0xAC, 0xE5, 0x9C, 0xB0, 0xE7, 0x94, 0x9F, 0xE6,
    0x88, 0x90, 0xE5, 0xAF, 0x86, 0xE9, 0x92, 0xA5,
    0xE5, 0xAF, 0xB9, 0x28, 0xE8, 0xA1, 0x8D, 0xE7
};

typedef struct __attribute__ ((__packed__)) {
    uint8_t     ver;
    uint8_t     orthocode;
    uint16_t    bodysize;
} Header;

void ortho_encode(Header& header, std::string& body)
{
    uint8_t code = header.orthocode ^ 0;
    for (size_t i = 0; i < body.size(); i++) {
        body[i] ^= code;
    }
}

void ortho_decode(Header& header, std::string& body)
{
    uint8_t code = header.orthocode ^ 0;
    for (size_t i = 0; i < body.size(); i++) {
        body[i] ^= code;
    }
}

int main()
{

    std::string str = std::string(ucs, ucs + sizeof(ucs));

    printf("str.size=%lu\n", str.size());
    
    Header header;
    header.ver = (uint8_t)'o';
    header.orthocode = rand() % 0xFE + 1;
    header.bodysize = str.size();
    
    ortho_encode(header, str);
    
    size_t i;
    for (i = 0; i < str.size(); i++) {
        printf("0x%02X, ", static_cast<int>((unsigned char)str[i]));
        if ((i + 1) % 8 == 0) printf("\n");
    }
    printf("\n");
    
    
    
    ortho_decode(header, str);
    for (i = 0; i < str.size(); i++) {
        printf("0x%02X, ", static_cast<int>((unsigned char)str[i]));
        if ((i + 1) % 8 == 0) printf("\n");
    }
    printf("\n");
    

    return 0;
}