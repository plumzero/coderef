
#include <string>
#include <fstream>
#include <sstream>

#include <iostream>

template<class CharT, class Traits, class Allocator>
std::basic_istream<CharT, Traits>& getall(std::basic_istream<CharT, Traits>& input,
                                          std::basic_string<CharT, Traits, Allocator>& str)
{
    std::ostringstream oss;
    oss << input.rdbuf();
    str.assign(oss.str());
    return input;
}

inline int get_file_content(const char* filename, std::string* out)
{
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (in) {
        getall(in, *out);
        return 0;
    } else {
        return -1;
    }
    in.close();
    return 0;
}

inline int set_file_content(const char* filename, const std::string& in)
{
    std::ofstream out(filename, std::ios::out | std::ios::binary);
    if (out) {
        out << in;
        out.close();
    } else {
        return -1;
    }
    return 0;
}
    

int main()
{       
    // 将字符流写入文件
    std::string in("Hello World!");
    set_file_content("hello.txt", in);
    
    // 从文件中读取内容到字符流
    std::string out;
    get_file_content("hello.txt", &out);
    
    std::cout << out << std::endl;
    
    return 0;
}