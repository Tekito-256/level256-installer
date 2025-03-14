#pragma once

#include <3ds/types.h>

class Network
{
public:
    static Result Initialize(void);
    static void Finalize(void);

    static Result Get(const char *url, const char *path);

private:
    static constexpr u32 SOC_SHARED_MEM_SIZE = 0x10000;

    static bool s_initialized;
    static u32 *s_socketSharedMem;
};