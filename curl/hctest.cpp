

#include "httpclient.h"
#include <iostream>
#include <fstream>
#include <map>

/**
 * g++ -g hctest.cpp httpclient.cpp -o hc  \
 *        -I/usr/local/libcurl/include -L/usr/local/libcurl/lib -lcurl \
 *        -ljsoncpp
 */
int main()
{
	/// GET method test 1
	try {
		int ret = 0;
		HttpClient hc;
		const char *url_get = "https://www.baidu.com/";
		std::string resp;
		std::ofstream ofs;
		
		ret = hc.methodGET(std::string(url_get), NULL, NULL, &resp);
		if (ret != 0) {
			goto geterr_1;
		}		
		ofs.open("GET_1.html", std::ofstream::out | std::ofstream::trunc);
		if (! ofs.is_open()) {
			goto geterr_1;
		}
		ofs << resp;
		
		geterr_1:
			ofs.close();
	}
	catch (HttpClient::HCStatus<HttpClient::HCS_ERR>)
	{
		printf("construct HttpClient error\n");
	}
	/// GET method test 2
	try {
		int ret = 0;
		HttpClient hc;
		const char *url_get = "mailman.nginx.org/pipermail/unit/2019-November/000187.html";
		std::string resp;
		std::ofstream ofs;
		std::map<std::string, std::string> params;
		std::string url(url_get);
		
		// params["_ga"] = "2.242055682.1412749566.1574651855-476520220.1573449728";
		
		ret = hc.methodGET(url, &params, NULL, &resp);
		if (ret != 0) {
			goto geterr_2;
		}		
		ofs.open("GET_2.html", std::ofstream::out | std::ofstream::trunc);
		if (! ofs.is_open()) {
			goto geterr_2;
		}
		ofs << resp;
		
		geterr_2:
			ofs.close();
		
	}
	catch (HttpClient::HCStatus<HttpClient::HCS_ERR>)
	{
		printf("construct HttpClient error\n");
	}
	
	return 0;
}
