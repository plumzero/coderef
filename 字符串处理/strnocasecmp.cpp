
#include <string>
#include <type_traits>
#include <algorithm>
#include <iostream>

template<typename S1, class = typename std::enable_if<std::is_convertible<S1, std::string>::value, std::string>::type,
         typename S2, class = typename std::enable_if<std::is_convertible<S2, std::string>::value, std::string>::type>
bool strnocasecmp(S1 s1, S2 s2)
{
    const std::string & l = s1;
    const std::string & r = s2;
    
    if (l.size() != r.size()) {
        return false;
    }
    
    return std::equal(l.begin(), l.end(), r.begin(), [](const unsigned char & c1, const unsigned char & c2) -> bool {
        return std::tolower(c1) == std::tolower(c2); 
    });
}

int main()
{
    std::cout << std::boolalpha;
    
    std::cout << strnocasecmp("Hello", "hello") << std::endl; 
    
    std::cout << strnocasecmp("hello", "hEllo") << std::endl;
    
    std::cout << strnocasecmp("Hello", "Hello") << std::endl;
    
    std::cout << strnocasecmp("World", "Hello") << std::endl;
    
    std::cout << strnocasecmp("Hello", "world") << std::endl;
    
    return 0;
}
