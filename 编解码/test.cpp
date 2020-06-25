
#include "base64.h"
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#define USAGE                                              \
        "./program <src file> <des file> <-e or -d>"   \
        "\n"

int main(int argc, char* argv[])
{
    if (argc != 4) {
        std::cerr << USAGE << std::endl;
        return -1;
    }
    
    int encode;
    if (! strcmp(argv[3], "-e")) {
        encode = 1;
    } else if (! strcmp(argv[3], "-d")) {
        encode = 0;
    } else {
        std::cerr << USAGE << std::endl;
        return -1;
    }
    
    std::ifstream ifs(argv[1], std::ifstream::in | std::ifstream::binary);
    if (! ifs.is_open()) {
        std::cerr << "open " << argv[1] << " for reading failed" << std::endl;
        return -1;
    }
    std::ostringstream oss;
    oss << ifs.rdbuf();
    ifs.close();
    
    std::string stream;
    if (encode) {
        stream = base64_encode(oss.str());
    } else {
        stream = base64_decode(oss.str());
    }
    
    std::ofstream ofs(argv[2], std::ofstream::out | std::ofstream::binary);
    if (! ofs.is_open()) {
        std::cerr << "open " << argv[1] << " for writing failed" << std::endl;
        return -1;
    }
    ofs << stream;
    ofs.close();
    
    return 0;
}