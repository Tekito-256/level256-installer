#include "Network.hpp"

#include <3ds.h>
#include <malloc.h>
#include <curl/curl.h>

bool Network::s_initialized = false;
u32 *Network::s_socketSharedMem = nullptr;

Result Network::Initialize(void)
{
    if (s_initialized)
        return -1;

    Result res = 0;

    s_socketSharedMem = (u32 *)memalign(0x1000, SOC_SHARED_MEM_SIZE);
    if (s_socketSharedMem == nullptr)
        return -2;

    res = socInit(s_socketSharedMem, SOC_SHARED_MEM_SIZE);
    if (res < 0)
    {
        free(s_socketSharedMem);
        s_socketSharedMem = nullptr;
        return res;
    }

    s_initialized = true;
    return res;
}

void Network::Finalize(void)
{
    if (!s_initialized)
        return;

    socExit();
    free(s_socketSharedMem);
    s_socketSharedMem = nullptr;
    s_initialized = false;
}

Result Network::Get(const char *url, const char *path)
{
    CURL *curl = curl_easy_init();
    if (curl == nullptr)
        return -1;

    FILE *fp = fopen(path, "wb");
    if (fp == nullptr)
    {
        curl_easy_cleanup(curl);
        return -2;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Nintendo 3DS/cURL");
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    Result res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    fclose(fp);

    return res;
}