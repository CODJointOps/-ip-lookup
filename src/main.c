#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>

#define VPN_DB_DIR "/tmp/vpndb"
#define VPN_LIST_PATH VPN_DB_DIR "/ipv4.txt"

#define VPN_LIST_URL "https://raw.githubusercontent.com/X4BNet/lists_vpn/main/ipv4.txt"

int ensure_directory_exists(const char *dirPath) {
    struct stat st = {0};

    if (stat(dirPath, &st) == -1) {
        if (mkdir(dirPath, 0755) == -1) {
            fprintf(stderr, "Failed to create directory %s\n", dirPath);
            return -1;
        }
    }
    return 0;
}

unsigned long ip_to_ulong(const char *ip) {
    struct in_addr inVal;
    if (inet_aton(ip, &inVal)) {
        return ntohl(inVal.s_addr);
    }
    return 0;
}

int ip_in_cidr(const char *ip, const char *cidr) {
    unsigned long ip_addr = ip_to_ulong(ip);
    int mask;
    char cidr_ip[INET_ADDRSTRLEN];

    sscanf(cidr, "%[^/]/%d", cidr_ip, &mask);
    unsigned long cidr_addr = ip_to_ulong(cidr_ip);
    unsigned long cidr_mask = mask ? (~0 << (32 - mask)) : 0;

    return (ip_addr & cidr_mask) == (cidr_addr & cidr_mask);
}

int is_vpn(const char *ip_address) {
    FILE *file = fopen(VPN_LIST_PATH, "r");
    if (!file) {
        fprintf(stderr, "Cannot open VPN list file.\n");
        return -1;
    }

    char line[INET_ADDRSTRLEN + 4];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        if (ip_in_cidr(ip_address, line)) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

static size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

int download_vpn_list(const char *url, const char *output_path) {
    printf("Downloading VPN lists...\n");

    struct stat fileInfo;
    time_t now;
    double hours;

    time(&now);

    if (stat(output_path, &fileInfo) == 0) {
        hours = difftime(now, fileInfo.st_mtime) / (60.0 * 60.0);

        if (hours < 24.0) {
            return 0;
        }
    }

    CURL *curl;
    FILE *fp;
    CURLcode res;
    int result = 1; // Assume failure

    curl = curl_easy_init();
    if (curl) {
        fp = fopen(output_path, "wb");
        if (fp == NULL) {
            fprintf(stderr, "Cannot open file %s\n", output_path);
            curl_easy_cleanup(curl);
            return result;
        }

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(curl);

        if (res == CURLE_OK) {
            result = 0; // Success
        } else {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        fclose(fp);
        curl_easy_cleanup(curl);
    }
    return result;
}

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

    if (ensure_directory_exists(VPN_DB_DIR) != 0) {
        fprintf(stderr, "Failed to create VPN database directory.\n");
        return 1;
    }

    if (download_vpn_list(VPN_LIST_URL, VPN_LIST_PATH) != 0) {
        fprintf(stderr, "Failed to download VPN list.\n");
        return 1;
    }

    int vpnCheck = is_vpn(argv[1]);

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
    printf("VPN: %s\n", vpnCheck ? "True" : "False");

    cJSON_Delete(json);
    return 0;
}
