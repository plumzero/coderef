
#include <string>
#include <algorithm>
#include <string.h>

const std::string cert =
"-----BEGIN CERTIFICATE-----\n"
"MIIC5TCCAc0CFGU2cLR5y0Wpcvotm2ZrbfkauQK0MA0GCSqGSIb3DQEBCwUAMC8x\n"
"DjAMBgNVBAYMBUNISU5BMQwwCgYDVQQKDANCWUQxDzANBgNVBAMMBlNOLVZJTjAe\n"
"Fw0yMzExMDYxMDM2MzRaFw0yNDExMDUxMDM2MzRaMC8xDjAMBgNVBAYMBUNISU5B\n"
"MQwwCgYDVQQKDANCWUQxDzANBgNVBAMMBlNOLVZJTjCCASIwDQYJKoZIhvcNAQEB\n"
"BQADggEPADCCAQoCggEBALj8z3VMjege7mZ/zjxeY+02+nadMErOwnLOhvWa/Qlc\n"
"gYsJiwUPS93kXu2ZEYmq0eGMdYic6BdnT9KMNcmNV55zbiOIdOWpg3osWQkbtZqC\n"
"XQIhFwB0if9pYKtLAzrQcAzcxdBsAKH+NNyspfo/LF241iQ2qpu52MhgAf0RqNZI\n"
"MrQK+RofTAG2pAVMevlQ8L0lYF0aiOtBHdAWtYxTm+6h2eYDUz6g6NJ8O968OD0m\n"
"fZC4s8Egd+a9v2LE6Jjz0UIOmekHjc3IjYxERS7z2C5aeEvYXGwyUzOc1rU2ixpa\n"
"/ft2Um/KPsTrYWwZu9CeoHrplYpVH7iBkXf6rx2GY/ECAwEAATANBgkqhkiG9w0B\n"
"AQsFAAOCAQEARZZNbus1Ozbj1hwheb+4dsdfGC/7Kms+j2VzctTvqTmC8wm35cO8\n"
"iHYaCMW1h3tCu+VJcNvXolu2rucMb1wwxJ8jrAQMvxZHlpoaXDTm6BmIjtyaKFT8\n"
"Nqks+5Y2aE3mQRgqwZFtIWVbkrqh45B8NbuyTapVvEpBKgDuJ7GJFpCygC56Ri3M\n"
"8Luc9j0N5mzgIMiVvaYYey+vr7Vc6WFFaG3LctG5RfkDPf2Fwb8Y7ES2ZilufegF\n"
"FjL3sTu2Bp36ITddsq8mrAxdGEY4n3bT/hGYYiSufYh54KZ2S+DEkt0QndlUahCd\n"
"muWmArS5PQneYjNvLhToV25DXoyhUOfXnQ==\n"
"-----END CERTIFICATE-----";

std::string remove_hf(const std::string& cert)
{
  const std::string header = "-----BEGIN CERTIFICATE-----";
  const std::string footer = "-----END CERTIFICATE-----";

  const char* pheader = strstr(cert.c_str(), header.c_str());
  if (pheader == nullptr) {
    return "";
  }
  
  const char* pfooter = strstr(cert.c_str(), footer.c_str());
  if (pfooter == nullptr || pfooter <= pheader) {
    return "";
  }

  pheader += header.size();

  std::string content;
  for (const char* pos = pheader; pos != pfooter; pos++) {
    if (*pos != ' ' && *pos != '\r' && *pos != '\n') content.push_back(*pos);
  }

  return content;
}

int main()
{
  std::string content = remove_hf(cert);

  printf("%s", content.c_str());

  return 0;
}