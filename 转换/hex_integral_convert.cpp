
#include <stdint.h>
#include <iostream>
#include <string>
#include <algorithm>

inline std::string bin2hex(const std::string& in)
{
    std::string out;
    
    for (const auto & ele : in) {
        out.append(1, "0123456789ABCDEF"[static_cast<int>((unsigned char)ele) / 16]);
        out.append(1, "0123456789ABCDEF"[static_cast<int>((unsigned char)ele) % 16]);
    }
    
    return out;
}

inline std::string hex2bin(const std::string& in)
{
    std::string out(in.size() / 2, '\x0');
    
    char ch, ck;
    int i = 0;
    
    for (const auto & ele : in) {
        if (ele >= '0' && ele <= '9') ch = ele - '0'; else
        if (ele >= 'A' && ele <= 'F') ch = ele - '7'; else
        if (ele >= 'a' && ele <= 'f') ch = ele - 'W'; else
            return "";
        
        ck = ((i & 1) != 0) ? ch : ch << 4;
        
        out[i >> 1] = (unsigned char)(out[i >> 1] | ck);
        i++;
    }
    
    return out;
}

/** 整型精确转十六进制字符串(大端存储) */
template<typename T, class = typename std::enable_if<std::is_integral<T>::value>::type>
std::string integral2hex(const T& t)
{    
    const unsigned LE = 1;
    unsigned isLittleEndian = *((char*)&LE);
    
    std::string ts(reinterpret_cast<const char*>((unsigned char*)&t), sizeof(t));
    
    size_t b, e;
    
    isLittleEndian ? (b = 0, e = ts.find_last_not_of('\x0') + 1)
                   : (b = ts.find_first_not_of('\x0'), e = sizeof(t) + 1);

    std::string bin(ts.begin() + b, ts.begin() + e);
        
    // 按大端存储
    isLittleEndian ? std::reverse(bin.begin(), bin.end()) : void(0);
    
    return bin.empty() ? "00" : bin2hex(bin);
}

/** 十六进制字符串(大端存储)精确转整型 */
template<typename T, class = typename std::enable_if<std::is_integral<T>::value>::type,
         typename S, class = typename std::enable_if<std::is_convertible<S, std::string>::value, std::string>::type>
T hex2integral(const S& hex)
{    
    const unsigned LE = 1;
    unsigned isLittleEndian = *((char*)&LE);
    
    std::string bin(hex2bin(hex));

    isLittleEndian ? std::reverse(bin.begin(), bin.end()) : void(0);

    isLittleEndian ? bin.append(sizeof(T) - bin.size(), '\x0')
                   : bin.insert(0, sizeof(T) - bin.size(), '\x0');

    return *reinterpret_cast<const T*>(bin.c_str());
}

int main()
{
    std::string hex;
    {
        /** [1] 内建类型测试 */
        std::cout << "内建类型测试:" << std::endl;
        char c = '\x65';
        hex = integral2hex(c);
        std::cout << hex2integral<char>(hex) << std::endl;
        
        unsigned char uc = 0x65;
        hex = integral2hex(uc);
        std::cout << hex2integral<unsigned char>(hex) << std::endl;
        
        signed char sc = -97;
        hex = integral2hex(sc);
        std::cout << (int)hex2integral<signed char>(hex) << std::endl;
        
        short s16 = -123;
        hex = integral2hex(s16);
        std::cout << hex2integral<short>(hex) << std::endl;
        
        unsigned short us16 = 456;
        hex = integral2hex(us16);
        std::cout << hex2integral<unsigned short>(hex) << std::endl;
        
        int i32 = -6789;
        hex = integral2hex(i32);
        std::cout << hex2integral<int>(hex) << std::endl;
        
        unsigned int ui32 = 9876;
        hex = integral2hex(ui32);
        std::cout << hex2integral<unsigned int>(hex) << std::endl;
        
        long l = -1234;
        hex = integral2hex(l);
        std::cout << hex2integral<long>(hex) << std::endl;
        
        unsigned long ul = 6789;
        hex = integral2hex(ul);
        std::cout << hex2integral<unsigned long>(hex) << std::endl;

        long long ll = -12345678987654;
        hex = integral2hex(ll);
        std::cout << hex2integral<long long>(hex) << std::endl;
        
        unsigned long long ull = 98765432101234;
        hex = integral2hex(ull);
        std::cout << hex2integral<unsigned long long>(hex) << std::endl;
    }
    
    {
        /** [2] 标准库定义类型测试 */
        std::cout << "标准库定义类型测试:" << std::endl;
        int8_t i8 = -123;
        hex = integral2hex(i8);
        std::cout << (int)hex2integral<int8_t>(hex) << std::endl;
        
        uint8_t ui8 = 254;
        hex = integral2hex(ui8);
        std::cout << (int)hex2integral<uint8_t>(hex) << std::endl;
        
        int16_t i16 = -123;
        hex = integral2hex(i16);
        std::cout << hex2integral<int16_t>(hex) << std::endl;
        
        uint16_t ui16 = 456;
        hex = integral2hex(ui16);
        std::cout << hex2integral<uint16_t>(hex) << std::endl;
        
        int32_t i32 = -6789;
        hex = integral2hex(i32);
        std::cout << hex2integral<int32_t>(hex) << std::endl;
        
        uint32_t ui32 = 9876;
        hex = integral2hex(ui32);
        std::cout << hex2integral<uint32_t>(hex) << std::endl;
        
        int64_t i64 = -12345678987654;
        hex = integral2hex(i64);
        std::cout << hex2integral<int64_t>(hex) << std::endl;
        
        uint64_t ui64 = 9876543210123456;
        hex = integral2hex(ui64);
        std::cout << hex2integral<uint64_t>(hex) << std::endl;
    }
    
    {
        /** [3] 特殊数据测试 */
        std::cout << "特殊数据测试:" << std::endl;
        int i32 = 0;
        hex = integral2hex(i32);
        std::cout << hex << std::endl;
        std::cout << hex2integral<int>(hex) << std::endl;
        
        hex.clear();
        std::cout << hex2integral<int>(hex) << std::endl;
    }
    
    {
        /** [4] C-style 字符串测试 */
        std::cout << "C-style 字符串测试:" << std::endl;
        char chs[20] = "4CEF";
        std::cout << hex2integral<uint32_t>(chs) << std::endl;
        char zero[2] = { '\x0', '\x0' };
        std::cout << hex2integral<uint32_t>(zero) << std::endl;
    }
    
    return 0;
}
