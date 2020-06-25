
#include <string>
#include <iostream>
#include <algorithm>

void ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

void rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

static inline void trim(std::string &s)
{
    ltrim(s);
    rtrim(s);
}

/** 文本处理时可能会用到 */
    
int main()
{

    std::string stream = R"(        D:\Program Files (x86)\AutoCAD 2008\drv\gdiplot9.hdi          )";
    std::cout << "before: " << stream << std::endl;
    
    trim(stream);    
    std::cout << "after: " << stream << std::endl;
    
    return 0;
}