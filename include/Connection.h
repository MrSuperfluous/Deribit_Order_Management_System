#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>
#include <unordered_map>
#include "rapidjson/document.h"

class Connection {
private:
    std::string baseUrl;
public:
    Connection(const std::string& baseUrl);

    rapidjson::Document sendRequest(
        const std::string& endpoint,
        const std::unordered_map<std::string, std::string>& params,
        const std::string& method,
        const std::string& token = "");

private:
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
};

#endif
