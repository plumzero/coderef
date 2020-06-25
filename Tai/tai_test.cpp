

#include "tai.h"

int main()
{
    {
        TC::Tai tai;
        tai.printTime();
        
        tai.addYears(1).addMonths(5);
        tai.printTime();
    }
    
    {
        TC::Tai tai;
        tai.printTime();
        tai.addWeeks(8);
        tai.printTime();
    }
    {
        TC::Tai tai;
        tai.printTime();
        tai.addHours(5);
        tai.printTime();
    }
    
    {
        TC::Tai tai("2020-2-29 18:22:08");
        tai.printTime();
        tai.addMonths(5);
        tai.printTime();
    }
    
    {
        TC::Tai tai("2020-2-29 18:22:08");
        tai.printTime();
        tai.addDays(5);
        tai.printTime();
    }
    
    {
        uint64_t time;

        time = TC::Tai();
        std::cout << "time: " << time << std::endl;
        
        time = TC::Tai().addMinutes(20);
        std::cout << "time: " << time << std::endl;
        
        time = TC::Tai().addSeconds(59);
        std::cout << "time: " << time << std::endl;
    }
    
    {
        TC::Tai tai;
        tai.printTime();
        tai.addMonths(13);
        tai.printTime();
    }
    {
        TC::Tai tai;
        tai.printTime();
        tai.addMonths(18);
        tai.printTime();
    }
    {
        TC::Tai tai;
        tai.printTime();
        tai.addMonths(21);
        tai.printTime();
    }
    {
        TC::Tai tai;
        tai.printTime();
        tai.addMonths(22);
        tai.printTime();    
    }
    /**
        13          18          21          22
        21.4.16     21.9.16     21.12.16    22.1.16
        
     */
    {
        TC::Tai tai;
        tai.printTime();
        tai.addYears(5);
        tai.printTime();    
    }
    {
        TC::Tai tai("2020-2-29 18:22:08");
        tai.printTime();
        tai.addYears(2);
        tai.printTime();
    }
    {
        TC::Tai tai("2020-2-29 18:22:08");
        tai.printTime();
        tai.addMonths(24);
        tai.printTime();        
    }
    
    return 0;
}