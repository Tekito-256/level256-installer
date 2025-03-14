#pragma once

#include <3ds/types.h>
#include <string>

class Installer
{
public:
    Installer(void);
    ~Installer(void);

    bool Install(void);

private:
    Result GetLatestAssetUrl(std::string &url);
    Result DownloadZip(const char *url, const char *path);
    bool ExtractZip(const char *path, const char *destDir);
    bool InstallFiles(const std::string &filesDir);
    bool CreateDirectories(void);
    bool CreateDirIfNotExist(const char *path);
};