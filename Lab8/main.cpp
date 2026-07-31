#include <iostream>
#include <string>
#include <curl/curl.h>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " <username> <message>\n";
        return 1;
    }

    CURL *curl = curl_easy_init();

    if (!curl)
    {
        std::cout << "Failed to initialise curl\n";
        return 1;
    }

    char *escaped = curl_easy_escape(curl, argv[2], 0);

    std::string url =
        "http://api.thingspeak.com/update?"
        "api_key=ZKE95ZURWV7DWB80"
        "&field1=" + std::string(argv[1]) +
        "&field2=" + std::string(escaped);

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK)
    {
        std::cout << curl_easy_strerror(res) << std::endl;
    }

    curl_free(escaped);
    curl_easy_cleanup(curl);

    return 0;
}

