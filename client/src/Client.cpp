#include <windows.h>

#include "app/Application.hpp"

/**
 * @file Client.cpp
 * @brief Process entry point: creates the Application and runs it.
 */

/** @brief Global pointer to the running Application instance. */
static Application* _App = nullptr;

/**
 * @brief Windows application entry point.
 * @param hInstance Process instance handle.
 * @param nCmdShow Initial show-window command.
 * @return The process exit code returned by Application::Run().
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    Application app(hInstance, nCmdShow);
    _App = &app;
    return app.Run();
}