
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <utility>

std::vector<std::string> split(const std::string& str, char sep)
{
    std::vector<std::string> res;
    auto start = str.begin();
    auto end = str.end();
    auto next = std::find(start, end, sep);

    while (next != end)
    {
        res.emplace_back(start, next);
        start = next + 1;
        next = std::find(start, end, sep);
    }

    res.emplace_back(start, next);
    
    return res;
}

std::vector<std::string> split_filter_empty(const std::string& str, char sep)
{
    std::vector<std::string> res;
    auto start = str.begin();
    auto end = str.end();
    auto next = find(start, end, sep);

    while (next != end)
    {
        if (start < next)
            res.emplace_back(start, next);

        start = next + 1;
        next = find(start, end, sep);
    }

    if (start < next)
        res.emplace_back(start, next);

    return res;
}

int main()
{
    const std::string& str = "Hello, foo, I am bar ,";

    {
        std::cout << "First test:" << std::endl;
        std::vector<std::string> vs = std::move(split(str, ','));
        
        for (const auto & ele : vs) {
            std::cout << "{" << ele << "}" << std::endl;
        }
    }
    
    {
        std::cout << "Second test:" << std::endl;
        std::vector<std::string> vs = std::move(split_filter_empty(str, ','));
        
        for (const auto & ele : vs) {
            std::cout << "{" << ele << "}" << std::endl;
        }
    }

    return 0;
}