#ifndef __HTTP_CLIENT_WITH_CURL_H__
#define __HTTP_CLIENT_WITH_CURL_H__

#include <curl/curl.h>
#include <string>
#include <map>

class HttpClient
{
private:
    CURL *curl;
    int debug;
    int connect_timeout;
    int socket_timeout;
public:
    typedef enum { HCS_OK, HCS_ERR, HCS_DISCONN } Status;
    template <Status T>  class HCStatus {};
public:
    HttpClient();
    HttpClient(const HttpClient &) = delete;
    HttpClient &operator=(const HttpClient &) = delete;
    ~HttpClient();
    inline void setConnectTimeout(int connect_timeout)
    {
        this->connect_timeout = connect_timeout;
    }
    
    inline void setSocketTimeout(int socket_timeout)
    {
        this->socket_timeout = socket_timeout;
    }
    
    inline void setDebug(int debug)
    {
        this->debug = debug;
    }
    void makeUrlencodedForm(std::map<std::string, std::string> const &params,
                            std::string *content) const;
    void appendUrlParams(std::map<std::string, std::string> const &params, 
                         std::string *url) const;
    void appendHeaders(std::map<std::string, std::string> const &headers,
                       curl_slist **slist) const;
    int methodGET(std::string url, 
                  std::map<std::string, std::string> const *params, 
                  std::map<std::string, std::string> const *headers, 
                  std::string *resp) const;
    int methodPost(std::string url,
                   std::map<std::string, std::string> const *params,
                   std::string const &body,
                   std::map<std::string, std::string> const *headers,
                   std::string *resp) const;
    int methodPost(std::string url,
                   std::map<std::string, std::string> const *params,
                   std::map<std::string, std::string> const &data,
                   std::map<std::string, std::string> const *headers,
                   std::string *resp) const;
    int methodPost(std::string url,
                   std::map<std::string, std::string> const *params,
                   std::map<std::string, std::string> const *headers,
                   std::string *resp) const;
private:  
    static inline size_t write_callback(void *buffer,
                                        size_t size,
                                        size_t nmemb,
                                        void *userp)
    {
        std::string *str = dynamic_cast<std::string*>((std::string *)userp);
        str->append((char *)buffer, size *nmemb);
        return nmemb;
    }
};

#endif  // !__HTTP_CLIENT_WITH_CURL_H__