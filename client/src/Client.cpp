#include <windows.h>

#include "app/Application.hpp"

static Application* _App = nullptr;

BOOL WINAPI CtrlHandler(DWORD signal)
{
    if (signal == CTRL_C_EVENT && _App)
    {
        _App->Shutdown();
        return TRUE;
    }
    return FALSE;
}

int WINAPI main(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    Application app(hInstance, nCmdShow);
    _App = &app;
    SetConsoleCtrlHandler(CtrlHandler, TRUE);
    return app.Run();
}