#include "Connection.h"
#include <iostream>
#include <curl/curl.h>
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"
#include <mutex>

// Constructor to store the base URL
Connection::Connection(const std::string& baseUrl) : baseUrl(baseUrl) {} 

// Callback function for writing received data to a string
size_t Connection::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    // Calculate total size of received data
    size_t totalSize = size * nmemb; 
    // Cast user data pointer to string pointer
    std::string* response = static_cast<std::string*>(userp); 
    // Append received data to the string
    response->append(static_cast<char*>(contents), totalSize); 
    return totalSize; // Return the number of bytes processed
}

// Send a request to the server and parse the JSON response
rapidjson::Document Connection::sendRequest(
    const std::string& endpoint, 
    const std::unordered_map<std::string, std::string>& params, 
    const std::string& method, 
    const std::string& token) {

    CURL* curl; // Handle for libcurl
    CURLcode res; // Result code from libcurl operations

    // Construct the full URL
    std::string url = baseUrl + endpoint; 

    // Initialize curl
    curl = curl_easy_init(); 
    if (!curl) {
        std::cerr << "curl_easy_init() failed!" << std::endl;
        return rapidjson::Document(); // Return empty document on error
    }

    // Create JSON data for POST requests
    std::string data = "{"; 
    for (const auto& param : params) {
        data += "\"" + param.first + "\": \"" + param.second + "\",";
    }
    if (!params.empty()) {
        data.pop_back(); // Remove trailing comma
    }
    data += "}";

    // String to store the response from the server
    std::string response_string; 

    // Set the URL for the request
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    // Set request method (POST, GET, etc.)
    if (method == "POST") {
        curl_easy_setopt(curl, CURLOPT_POST, 1L); 
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.length()); 
    } else if (method == "GET") {
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L); 
        if (!params.empty()) {
            url += "?"; 
            for (const auto& param : params) {
                url += param.first + "=" + param.second + "&";
            }
            url.pop_back(); 
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); 
        }
    } else {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str()); 
    }

    // Set callback function for writing received data
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); 
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string); 

    // Set HTTP headers (if any)
    struct curl_slist* headers = nullptr; 
    if (!token.empty()) {
        std::string auth_header = "Authorization: Bearer " + token;
        headers = curl_slist_append(headers, auth_header.c_str());
    }
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Perform the request
    res = curl_easy_perform(curl); 

    if (res != CURLE_OK) {
        std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        curl_slist_free_all(headers); 
        curl_easy_cleanup(curl); 
        return rapidjson::Document(); // Return empty document on error
    }

    // Parse the JSON response
    rapidjson::Document doc; 
    rapidjson::ParseResult ok = doc.Parse(response_string.c_str());
    if (!ok) {
        std::cerr << "JSON Parse error: " << rapidjson::GetParseError_En(ok.Code()) 
                  << " (" << ok.Offset() << ")" << std::endl;
        std::cerr << "Response String: " << response_string << std::endl; 
        curl_slist_free_all(headers); 
        curl_easy_cleanup(curl); 
        return rapidjson::Document(); // Return empty document on error
    }

    // Clean up
    curl_slist_free_all(headers); 
    curl_easy_cleanup(curl); 

    return doc;
}