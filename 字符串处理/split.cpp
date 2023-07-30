
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <utility>
#include <map>

std::vector<std::string> split(const std::string& str, char sep)
{
    if (str.empty()) return {};

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
    if (str.empty()) return {};

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

std::map<std::string, std::string> split_to_map(const std::string& str, char sep, char kvsep)
{
    std::map<std::string, std::string> res;
    auto start = str.begin();
    auto end = str.end();
    auto next = std::find(start, end, sep);
    
    while (next != end)
    {
        if (start < next) {
            auto mid = std::find(start, next, kvsep);
            if (start != mid && mid != next) {
                res.insert(std::make_pair(std::string(start, mid), std::string(mid + 1, next)));
            }
        }

        start = next + 1;
        next = std::find(start, end, sep);
        
    }

    return res;
}

int main()
{
    const std::string& str = "accountid:8001,symbol:000768.SZ,cfgvol:,buyvol:200,sellvol:200,buyamount:6994.000000,sellamount:6976.000000,floatprofit:0.000000,settleprofit:-26.800000,fee:8.800000,server:11,";
    

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
    
    {
        std::cout << "Third test:" << std::endl;
        std::map<std::string, std::string> m = std::move(split_to_map(str, ',', ':'));
        
        for (const auto & ele : m) {
            std::cout << ele.first << " => " << ele.second << std::endl;
        }
    }
    return 0;
}