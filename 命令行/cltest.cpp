
#include "cldetect.h"
#include <iostream>

int main(int argc, const char* argv[])
{
    CmdLine args(argc, argv);

    // std::cout << "size: " << args.size() << std::endl;
    // std::cout << "str: " << args.str() << std::endl;
    
    // for (const auto &it : args.clmap) {
        // std::cout << it.first << " : " << it.second << std::endl;
    // }
    
    std::cout << std::boolalpha;
    
    auto help = getArgs(false, "-h", "--help", "-?");
    std::cout << "help=" << help << std::endl;
    auto version = getArgs("", "-v", "--version", "--show-version");
    std::cout << "version=" << version << std::endl;
    auto depth = getArgs(0, "-d", "--depth", "--max-depth");
    std::cout << "depth=" << depth << std::endl;
    auto file = getArgs("", "-f", "--file");
    std::cout << "file=" << file << std::endl << std::endl;
    
    std::cout << args["--file"] << std::endl;
    
    return 0;
}