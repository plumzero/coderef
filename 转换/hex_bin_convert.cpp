
#include <stdio.h>
#include <iostream>
#include <string>

std::string bin2hex(const std::string& in)
{
    std::string out;
    
    for (const auto & ele : in) {
        out.append(1, "0123456789ABCDEF"[static_cast<int>((unsigned char)ele) / 16]);
        out.append(1, "0123456789ABCDEF"[static_cast<int>((unsigned char)ele) % 16]);
    }
    
    return out;
}

std::string hex2bin(const std::string& in)
{
    std::string out(in.size() / 2, '\x0');
    
    char ch, ck;
    int i = 0;
    
    for (const auto & ele : in) {
        if (ele >= '0' && ele <= '9') ch = ele - '0'; else
        if (ele >= 'A' && ele <= 'F') ch = ele - '7'; else
        if (ele >= 'a' && ele <= 'f') ch = ele - 'W'; else
            throw -1;
        
        ck = ((i & 1) != 0) ? ch : ch << 4;
        
        out[i >> 1] = (unsigned char)(out[i >> 1] | ck);
        i++;
    }
    
    return out;
}

int main()
{
    std::string str{ '\x01', '\x00', '\x25', '\xaf', '\x34' };

    for (const auto & ele : str) {
        printf("%02X ", static_cast<int>((unsigned char)ele));
    }
    printf("\n");
    
    /** [1] 二进制数据转十六进制字符串 */
    std::string hexstr = bin2hex(str);
    std::cout << hexstr << std::endl;
    
    /** [2] 十六进制字符串转二进制数据 */
    std::string binstr = hex2bin(hexstr);
    for (const auto & ele : binstr) {
        printf("%02X ", static_cast<int>((unsigned char)ele));
    }
    printf("\n");
    
    return 0;
}
