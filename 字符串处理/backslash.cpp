
#include <string>
#include <iostream>

static inline void backslash(std::string &s)
{
    size_t pos = 0;
    while ((pos = s.find_first_of('\\', pos)) != std::string::npos) {
        s.insert(pos, "\\");
        pos = pos + 2;
    }
}

/** 文本处理时可能会用到 */

int main()
{       
    std::string stream = R"(D:\Program Files (x86)\AutoCAD 2008\drv\gdiplot9.hdi)";
    std::cout << "before: " << stream << std::endl;
    
    backslash(stream);    
    std::cout << "after: " << stream << std::endl;
    
    return 0;
}