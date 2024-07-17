#include <iostream>
#include <string>
#include "curl/curl.h"
#include "json.hpp"

using json = nlohmann::json;

// Callback function to handle data received from cURL
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int main() {
    std::string latitude = "51.5074";  // Latitude for London
    std::string longitude = "-0.1278"; // Longitude for London
    std::string url = "https://api.open-meteo.com/v1/forecast?latitude=" + latitude + "&longitude=" + longitude + "&hourly=temperature_2m";

    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    if (!readBuffer.empty()) {
        // Print the raw JSON response
        std::cout << "API Response: " << readBuffer << std::endl;

        // Parse the JSON data
        try {
            auto jsonData = json::parse(readBuffer);

            // Print parsed JSON data
            std::cout << "Parsed JSON Data: " << jsonData.dump(4) << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        }
    } else {
        std::cerr << "Failed to retrieve data." << std::endl;
    }

    return 0;
}
