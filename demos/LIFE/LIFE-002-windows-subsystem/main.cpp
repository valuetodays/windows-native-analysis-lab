#include <windows.h>

int WINAPI wWinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    PWSTR pCmdLine,
    int nCmdShow)
{
    MessageBoxW(
        nullptr,
        L"Hello Windows Native Lab",
        L"LIFE-002",
        MB_OK
    );

    return 0;
}