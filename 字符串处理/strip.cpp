
#include <iostream>
#include <string>
#include <cctype>

std::string strip(const std::string& str)
{
    std::string res;

    if (!str.empty())
    {
        const char *st = str.c_str();
        const char *ed = st + str.size() - 1;

        while (st <= ed)
        {
            if (!isspace(*st))
                break;

            st++;
        }

        while (ed >= st)
        {
            if (!isspace(*ed))
                break;

            ed--;
        }

        if (ed >= st)
            res.assign(st, ed - st + 1);
    }

    return res;
}

int main()
{
    const std::string& str = "  Hello World		";
    
    std::cout << "{" << strip(str) << "}" << std::endl;
    
    return 0;
}