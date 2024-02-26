#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    ((char *)userp)[realsize] = 0;
    strcat(userp, contents);
    return realsize;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <IP_ADDRESS>\n", argv[0]);
        return 1;
    }

    char url[256] = "http://ip-api.com/json/";
    strcat(url, argv[1]);

    CURL *curl;
    CURLcode res;
    char buffer[2048] = {0};

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (!curl) {
        fprintf(stderr, "Error: Failed to initialize libcurl.\n");
        return 1;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer);

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        fprintf(stderr, "Error: Failed to fetch data from ip-api.com: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        return 1;
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();

    cJSON *json = cJSON_Parse(buffer);
    if (!json) {
        fprintf(stderr, "Error: Failed to parse JSON data.\n");
        return 1;
    }

    cJSON *status = cJSON_GetObjectItem(json, "status");
    if (status && cJSON_IsString(status) && strcmp(status->valuestring, "fail") == 0) {
        fprintf(stderr, "Error: Failed to fetch data for IP address %s\n", argv[1]);
        cJSON_Delete(json);
        return 1;
    }

    cJSON *city = cJSON_GetObjectItem(json, "city");
    cJSON *regionName = cJSON_GetObjectItem(json, "regionName");
    cJSON *country = cJSON_GetObjectItem(json, "country");
    cJSON *timezone = cJSON_GetObjectItem(json, "timezone");
    cJSON *zip = cJSON_GetObjectItem(json, "zip");
    cJSON *isp = cJSON_GetObjectItem(json, "isp");
    cJSON *org = cJSON_GetObjectItem(json, "org");
    cJSON *as = cJSON_GetObjectItem(json, "as");
    cJSON *hostname = cJSON_GetObjectItem(json, "hostName");


    printf("Hostname: %s\n", hostname ? hostname->valuestring : "Unknown");
    printf("City: %s\n", city ? city->valuestring : "Unknown");
    printf("Region: %s\n", regionName ? regionName->valuestring : "Unknown");
    printf("Country: %s\n", country ? country->valuestring : "Unknown");
    printf("Time Zone: %s\n", timezone ? timezone->valuestring : "Unknown");
    printf("ZIP: %s\n", zip ? zip->valuestring : "Unknown");
    printf("ISP: %s\n", isp ? isp->valuestring : "Unknown");
    printf("Organization: %s\n", org ? org->valuestring : "Unknown");
    printf("AS: %s\n", as ? as->valuestring : "Unknown");

    cJSON_Delete(json);
    return 0;
}
