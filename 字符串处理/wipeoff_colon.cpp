
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

/**
 * 在使用 wireshark 抓包时，有的版本抓包提取出的字节数据之间有 ':'，如果要分析数据，需要将 ':' 去掉。
 *      41:6f:47:42:41:6f:45:41:58:74:63:78:50:39:79:2b:66:59:67:42:49:44:67
 * 转换为:
 *      416f4742416f4541587463785039792b66596742494467
 */

int main(int argc, char* argv[])
{    
    if (argc != 3) {
        std::cerr << "Usage: ./exe <in file> <out file>" << std::endl;
        return -1;
    }
    
    std::ifstream ifs(argv[1], std::ifstream::in | std::ifstream::binary);
    if (! ifs.is_open()) {
        std::cerr << "open " << argv[1] << " failed" << std::endl;
        return -1;
    }
    
    std::string instream;
    ifs >> instream;
    ifs.close();
    
    std::string outstream(instream.begin(), std::remove(instream.begin(), instream.end(), ':'));
    
    std::ofstream ofs(argv[2], std::ofstream::out | std::ofstream::binary);
    if (! ofs.is_open()) {
        std::cerr << "open " << argv[2] << " failed" << std::endl;
        return -1;
    }
    
    ofs << outstream;
    ofs.close();

    return 0;
}