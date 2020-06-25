
#include "cldetect.h"
#include <iostream>
#include <stdlib.h>

#define HelpHint    \
    [&]() {     \
        std::cout <<"Version, " << CMDLINE_DETECT_VERSION << std::endl; \
        std::cout <<"Usage: detect [options] [paramter] ..." << std::endl; \
        std::cout << "Options:" << std::endl; \
        std::cout << "  -h, --help      display this help and exit" << std::endl; \
        std::cout << "  -v, --version   display version and exit" << std::endl; \
        }

int main( int argc, const char **argv )
{
    auto show_help = HelpHint;

    bool help = getArgs(false, "-h", "--help", "-?");
    int version = getArgs(0, "-v", "--version", "--show-version");
    int depth = getArgs(0, "-d", "--depth", "--max-depth");
    std::string file = getArgs("", "-f", "--file");

    if (help || argc <= 1) {
        show_help();
        return 0;
    }
    
    return 0;
}
