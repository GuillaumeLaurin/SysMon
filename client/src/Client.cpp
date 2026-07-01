#include <windows.h>

#include "app/Application.hpp"

static Application* _App = nullptr;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    Application app(hInstance, nCmdShow);
    _App = &app;
    return app.Run();
}