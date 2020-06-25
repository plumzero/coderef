
#ifndef __Cmd_Line_DETECT_H__
#define __Cmd_Line_DETECT_H__

#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define CMDLINE_DETECT_VERSION  ("1.0.0")

namespace Internal 
{
    template< typename T >
    inline T as(const std::string &self)
    {
        T t;
        return (std::istringstream(self) >> t) ? t :
               (T)(self.size() && (self != "0") && (self != "false"));
    }

    template<>
    inline char as(const std::string &self)
    {
        return self.size() == 1 ? (char)(self[0]) : (char)(as<int>(self));
    }
    template<>
    inline signed char as(const std::string &self)
    {
        return self.size() == 1 ? (signed char)(self[0]) : (signed char)(as<int>(self));
    }
    template<>
    inline unsigned char as(const std::string &self)
    {
        return self.size() == 1 ? (unsigned char)(self[0]) : (unsigned char)(as<int>(self));
    }
    template<>
    inline const char *as(const std::string &self)
    {
        return self.c_str();
    }
    template<>
    inline std::string as(const std::string &self)
    {
        return self;
    }

    inline size_t split(std::vector<std::string> &tokens, const std::string &self, const std::string &delimiters)
    {
        std::string str;
        
        tokens.clear();
        for (auto &ch : self) {
            if (delimiters.find_first_of(ch) != std::string::npos) {
                if (str.size()) {
                    tokens.push_back(str);
                    str = "";
                }
                tokens.push_back(std::string() + ch);
            } else {
                str += ch;
            }
        }
        return str.empty() ? tokens.size() : (tokens.push_back(str), tokens.size());
    };

    inline std::vector<std::string> cmdline()
    {
        std::vector<std::string> args;
        std::string arg;

        pid_t pid = getpid();

        char fname[32] = {};
        snprintf(fname, 32, "/proc/%d/cmdline", pid);
        std::ifstream ifs(fname);
        if (ifs.good()) {
            std::stringstream ss;
            ifs >> ss.rdbuf();
            arg = ss.str();
        }
        for (auto end = arg.size(), i = end - end; i < end; ++i) {
            auto st = i;
            while (i < arg.size() && arg[i] != '\0') ++i;
            args.push_back(arg.substr(st, i - st));
        }
        return args;
    }
}

class CmdLine
{
public:
    CmdLine(int argc, const char **argv)
    {
        std::vector<std::string> args(argc, std::string());
        int i;
        for (i = 0; i < argc; ++i) {
            args[i] = argv[i];
        }
        /** 建立映射表 */
        for (auto &it : args) {
            std::vector<std::string> tokens;
            auto size = Internal::split(tokens, it, "=");

            if (size == 3 && tokens[1] == "=") {
                clmap[tokens[0]] = tokens[2];         /** long parameter: key=value */
            } else if (size == 2 && tokens[1] == "=") {
                clmap[tokens[0]] = true;              /** exception case: --help= */
            } else if(size == 1 && tokens[0] != argv[0]) {
                clmap[tokens[0]] = true;              /** short parameter: opt=true*/
            }
        }
    }

    CmdLine(const std::vector<std::string> &args)
    {
        std::vector<const char *> argv;
        for (auto &it : args) {
            argv.push_back(it.c_str());
        }
        *this = CmdLine(argv.size(), argv.data());
    }

    size_t size() const
    {
        return clmap.size();
    }
    bool has(const std::string &op) const
    {
        return clmap.find(op) != clmap.end();
    }
    bool has(const char* op) const
    {
        return has(std::string(op));
    }

    std::string str() const
    {
        std::stringstream ss;
        std::string sep;
        for (auto &it : clmap) {
            ss << sep << it.first << "=" << it.second;
            sep = ',';
        }
        return ss.str();
    }
    
    operator std::map<std::string, std::string>() { return clmap; }
    std::string operator[](std::string key) {
        return clmap[key];
    }
    
public:
    std::map<std::string, std::string> clmap;
};

template<typename T>
inline T getArgs(const T &defaults, const char *argv)
{
    static CmdLine cl(Internal::cmdline());
    return cl.has(argv) ? Internal::as<T>(cl[argv]) : defaults;
}

template< typename T, typename... Args >
inline T getArgs(const T &defaults, const char *arg0, Args... argv)
{
    T t = getArgs<T>(defaults, arg0);
    return t == defaults ? getArgs<T>(defaults, argv...) : t;
}

inline const char * getArgs(const char *defaults, const char *argv)
{
    static CmdLine cl(Internal::cmdline());
    return cl.has(argv) ? Internal::as<const char *>(cl[argv]) : defaults;
}

template<typename... Args>
inline const char * getArgs(const char *defaults, const char *arg0, Args... argv)
{
    const char *t = getArgs(defaults, arg0);
    return t == defaults ? getArgs(defaults, argv...) : t;
}

#endif // !__Cmd_Line_DETECT_H__