#include "Installer.hpp"
#include "Network.hpp"
#include "Common.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <3ds.h>
#include <minizip/unzip.h>
#include <jansson.h>

Installer::Installer()
{
}

Installer::~Installer()
{
}

bool Installer::Install(void)
{
    Result res = 0;
    std::string latestAssetUrl;

    printf("Creating directories...\n");
    if (!this->CreateDirectories())
    {
        printf("Failed to create directories.\n");
        return false;
    }

    printf("Receiving latest asset URL...\n");
    res = this->GetLatestAssetUrl(latestAssetUrl);
    if (R_FAILED(res))
    {
        printf("Failed to receive latest asset URL.\nResult: %08lX\n", res);
        return false;
    }

    printf("Downloading latest asset...\n");
    res = this->DownloadZip(latestAssetUrl.c_str(), WORKING_DIRECTORY "latest.zip");
    if (R_FAILED(res))
    {
        printf("Failed to download latest asset.\nResult: %08lX\n", res);
        return false;
    }

    printf("Extracting latest asset...\n");
    if (!this->ExtractZip(WORKING_DIRECTORY "latest.zip", WORKING_DIRECTORY))
    {
        printf("Failed to extract latest asset.\n");
        return false;
    }

    printf("Installing...\n");
    if (!this->InstallFiles(WORKING_DIRECTORY))
    {
        printf("Failed to install files.\n");
        return false;
    }

    printf("Cleaning up...\n");
    remove(WORKING_DIRECTORY "latest.zip");
    remove(WORKING_DIRECTORY "latest.json");

    printf("\nDone!\n");

    return true;
}

Result Installer::GetLatestAssetUrl(std::string &url)
{
    Result res = 0;
    std::string dstPath = WORKING_DIRECTORY "latest.json";

    res = Network::Get("https://api.github.com/repos/Tekito-256/Level256_Network/releases/latest", dstPath.c_str());

    if (R_FAILED(res))
        return res;

    json_error_t err;
    json_t *json = json_load_file(dstPath.c_str(), 0, &err);

    if (json == NULL)
    {
        fprintf(stderr, "Failed to parse JSON: %s\n", err.text);
        return -1;
    }

    json_t *assets = json_object_get(json, "assets");
    if (assets == NULL)
    {
        fprintf(stderr, "Failed to find assets in JSON\n");
        return -1;
    }

    json_t *asset = json_array_get(assets, 0);
    if (asset == NULL)
    {
        fprintf(stderr, "Failed to find first asset in JSON\n");
        return -1;
    }

    json_t *browserDownloadUrl = json_object_get(asset, "browser_download_url");
    if (browserDownloadUrl == NULL)
    {
        fprintf(stderr, "Failed to find download URL in JSON\n");
        return -1;
    }

    const char *downloadUrl = json_string_value(browserDownloadUrl);
    if (downloadUrl == NULL)
    {
        fprintf(stderr, "Failed to get download URL from JSON\n");
        return -1;
    }

    url = std::string(downloadUrl);

    json_decref(json);

    return 0;
}

Result Installer::DownloadZip(const char *url, const char *path)
{
    return Network::Get(url, path);
}

bool Installer::ExtractZip(const char *path, const char *destDir)
{
    unzFile zip = unzOpen(path);
    if (zip == NULL)
    {
        fprintf(stderr, "Failed to open ZIP file: %s\n", path);
        return false;
    }

    if (unzGoToFirstFile(zip) != UNZ_OK)
    {
        fprintf(stderr, "Failed to find first file in ZIP\n");
        unzClose(zip);
        return false;
    }

    do
    {
        char filename[256];
        unz_file_info file_info;

        if (unzGetCurrentFileInfo(zip, &file_info, filename, sizeof(filename), NULL, 0, NULL, 0) != UNZ_OK)
        {
            fprintf(stderr, "Failed to get file info\n");
            break;
        }

        printf("Extracting: %s\n", filename);

        if (unzOpenCurrentFile(zip) != UNZ_OK)
        {
            fprintf(stderr, "Failed to open file in ZIP: %s\n", filename);
            continue;
        }

        FILE *fp = fopen((std::string(destDir) + filename).c_str(), "wb");
        if (!fp)
        {
            fprintf(stderr, "Failed to create file: %s\n", filename);
            unzCloseCurrentFile(zip);
            continue;
        }

        char buffer[8192];
        int bytes_read;
        while ((bytes_read = unzReadCurrentFile(zip, buffer, sizeof(buffer))) > 0)
        {
            fwrite(buffer, 1, bytes_read, fp);
        }

        fclose(fp);
        unzCloseCurrentFile(zip);
    } while (unzGoToNextFile(zip) == UNZ_OK);

    unzClose(zip);

    return true;
}

bool Installer::InstallFiles(const std::string &filesDir)
{
    // boot.firm
    remove(BOOTFIRM_PATH);
    if (rename((filesDir + "boot.firm").c_str(), BOOTFIRM_PATH) != 0)
        return false;

    // 3gx
    remove(PLUGIN_PATH);
    if (rename((filesDir + "Level256.3gx").c_str(), PLUGIN_PATH) != 0)
        return false;

    // Signature to enable plugin loader
    FILE *fp = fopen((LUMA_DIR + std::string("forceplgldr")).c_str(), "w");
    if (!fp)
        return false;
    fclose(fp);

    return true;
}

bool Installer::CreateDirectories(void)
{
    if (!this->CreateDirIfNotExist(WORKING_DIRECTORY))
        return false;

    if (!this->CreateDirIfNotExist(LUMA_DIR))
        return false;

    if (!this->CreateDirIfNotExist(PLUGIN_DIR))
        return false;

    if (!this->CreateDirIfNotExist(LEVEL256_DIR))
        return false;

    return true;
}

bool Installer::CreateDirIfNotExist(const char *path)
{
    if (access(path, F_OK) != 0)
        return mkdir(path, 0777) == 0;
    return true;
}