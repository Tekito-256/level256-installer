#include "Input.hpp"
#include "Network.hpp"
#include "Installer.hpp"

void CheckError(Result res, const char *msg)
{
    if (R_FAILED(res))
    {
        consoleInit(GFX_TOP, nullptr);
        printf("%s:\nAn error has occurred.\nResult: %08lX\n\nPress START to exit.", msg, res);
        while (aptMainLoop())
        {
            Input::Update();
            if (Input::IsPressed(Key::Start))
                break;
        }
    }
}

void Reboot(void)
{
    svcKernelSetState(7);
    svcExitThread();

    for (;;)
        ;
}

int main(void)
{
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);
    CheckError(Network::Initialize(), "Network::Initialize");

    printf("Press A to install Level256 Network.\n");
    printf("Press START to exit.\n");
    printf("\n");
    while (aptMainLoop())
    {
        Input::Update();

        if (Input::IsPressed(Key::Start))
            break;

        if (Input::IsPressed(Key::A))
        {
            if (Installer().Install())
            {
                printf("Rebooting...\n");
                gspWaitForVBlank();
                gfxSwapBuffers();
                Reboot();
            }
        }
        gspWaitForVBlank();
        gfxSwapBuffers();
    }

    Network::Finalize();
    gfxExit();
    return 0;
}