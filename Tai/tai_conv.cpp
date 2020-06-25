

#include "tai.h"
#include <string.h>
#include <string>

std::string tai_to_clock(uint64_t tai)
{
    struct tm *st;
    char tmbuf[64]{ 0 };
    time_t epoch;
    
    epoch = tai + 1072915200;
    
    st = localtime(&epoch);
    strftime(tmbuf, sizeof(tmbuf), "%Y-%m-%d %H:%M:%S", st);    
    
    return std::string(tmbuf, strlen(tmbuf));
}

int main()
{
    TC::Tai tai;
    tai.printTime();
    std::cout << tai_to_clock(tai) << std::endl;
        
    return 0;
}