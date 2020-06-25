

#include "httpclient.h"
#include <sstream>

/**
    try {
        HttpClient hc;
    }
    catch (HttpClient::HCStatus<HttpClient::HCS_OK>)
    {
        printf("hc construct ok\n");
    }
    catch (HttpClient::HCStatus<HttpClient::HCS_ERR>)
    {
        printf("hc construct err\n");
    }
    catch(...)
    {
        printf("hc construct status\n");
    }
 */
/**
 * curl_slist 的释放
 */
HttpClient::HttpClient()
{
    CURLcode res;
    
    res = curl_global_init(CURL_GLOBAL_ALL);
    if (res != CURLE_OK) {
        throw HCStatus<HCS_ERR>();
    }
    
    curl = curl_easy_init();
    if (curl == NULL) {
        throw HCStatus<HCS_ERR>();
    }
    
    this->connect_timeout = 10000;
    this->socket_timeout = 10000;
    this->debug = 0;
}

HttpClient::~HttpClient()
{
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}

void HttpClient::makeUrlencodedForm(std::map<std::string, std::string> const &params,
                                    std::string *content) const
{
    content->clear();
    
    std::map<std::string, std::string>::const_iterator it;
    
    for (it = params.begin(); it != params.end(); it++) {
        char *key = curl_easy_escape(curl, it->first.c_str(), it->first.size());
        char *val = curl_easy_escape(curl, it->second.c_str(), it->second.size());
        *content += key;
        *content += '=';
        *content += val;
        *content += '&';
        curl_free(key);
        curl_free(val);
    }
}

void HttpClient::appendUrlParams(std::map<std::string, std::string> const &params, 
                                 std::string *url) const
{
    if (params.empty())
        return;
    std::string content;
    this->makeUrlencodedForm(params, &content);
    bool url_has_param = false;
    if (std::string::npos != url->find('?'))
        url_has_param = true;
    if (url_has_param) {
        url->append("&");
    } else {
        url->append("?");
    }
    url->append(content);
}

void HttpClient::appendHeaders(std::map<std::string, std::string> const &headers,
                               curl_slist **slist) const
{
    std::ostringstream ostr;
    std::map<std::string, std::string>::const_iterator it;
    
    for (it = headers.begin(); it != headers.end(); it++) {
        ostr << it->first << ":" << it->second;
        *slist = curl_slist_append(*slist, ostr.str().c_str());
        ostr.str("");
    }
}

int HttpClient::methodGET(std::string url, 
                          std::map<std::string, std::string> const *params, 
                          std::map<std::string, std::string> const *headers, 
                          std::string *resp) const
{
    struct curl_slist *slist = NULL;
    
    if (headers) {
        this->appendHeaders(*headers, &slist);
    }
    if (params) {
        this->appendUrlParams(*params, &url);
    }
    
    printf("methodGET: %s\n", url.c_str());
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HttpClient::write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, static_cast<void *>(resp));
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, (long)1);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, this->connect_timeout);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, this->socket_timeout);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, (long)0);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, (long)0);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, (long)this->debug);
    
    CURLcode res = curl_easy_perform(curl);
    
    curl_slist_free_all(slist);
    
    return res;
}

int HttpClient::methodPost(std::string url,
                           std::map<std::string, std::string> const *params,
                           std::string const &body,
                           std::map<std::string, std::string> const *headers,
                           std::string *resp) const
{
    struct curl_slist *slist = NULL;
    
    if (headers) {
        this->appendHeaders(*headers, &slist);
    }
    if (params) {
        this->appendUrlParams(*params, &url);
    }
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
    curl_easy_setopt(curl, CURLOPT_POST, (long)1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.size());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HttpClient::write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, static_cast<void *>(resp));
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, (long)1);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, this->connect_timeout);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, this->socket_timeout);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, (long)0);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, (long)0);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, (long)this->debug);
    
    CURLcode res = curl_easy_perform(curl);
    
    curl_slist_free_all(slist);
    
    return res;
}

int HttpClient::methodPost(std::string url,
                           std::map<std::string, std::string> const *params,
                           std::map<std::string, std::string> const &data,
                           std::map<std::string, std::string> const *headers,
                           std::string *resp) const
{
    std::string body;
    
    this->makeUrlencodedForm(data, &body);
    
    return this->methodPost(std::move(url), params, body, headers, resp);
}

int HttpClient::methodPost(std::string url,
                           std::map<std::string, std::string> const *params,
                           std::map<std::string, std::string> const *headers,
                           std::string *resp) const
{   
    return this->methodPost(std::move(url), params, std::string(), headers, resp);
}
