#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include <windows.h>

#include "core/DIContainer.hpp"

#include "gui/Win32Window.hpp"

#include <memory>
#include <atomic>

/**
 * @file Application.hpp
 * @brief Declares the Application class, the root object owning the DI
 *        container, the main window and the application's run loop.
 */

/**
 * @brief Application root: wires every service into the DI container,
 *        owns the window and drives the render/logic loops.
 */
class Application
{
public:
    /**
     * @brief Stores the process launch parameters; real initialization happens in Run().
     * @param hInstance Process instance handle passed by WinMain.
     * @param nCmdShow Initial show-window command passed by WinMain.
     */
    Application(HINSTANCE hInstance, int nCmdShow);

    /**
     * @brief Initializes the services, runs the main loop and returns the process exit code.
     * @return Process exit code (0 on normal shutdown, non-zero on error).
     */
    uint32_t Run() noexcept;

    /** @brief Stops the background services and releases the UI resources. */
    void Shutdown() noexcept;

private:
    /** @brief Dependency-injection container owning every registered service. */
    DIContainer                  _Container;
    /** @brief True while the main loop should keep iterating. */
    std::atomic<bool>            _Running;
    /** @brief Process exit code returned by Run(). */
    uint32_t                     _Error;

    /** @brief Process instance handle passed by WinMain. */
    HINSTANCE _HInstance;
    /** @brief Initial show-window command passed by WinMain. */
    int _NCmdShow;
    /** @brief Main application window. */
    std::unique_ptr<Win32Window> _Window;

    /** @brief Background loop for non-UI work while the window is running. */
    void LogicLoop();

    /** @brief Registers every service in the container and starts the driver pipeline. */
    void Init();
};
