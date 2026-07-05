#include "app/Application.hpp"

#include "reporter/ErrorQueue.hpp"

#include "reporter/FileSink.hpp"

#include "reporter/JsonFormatter.hpp"

#include "reporter/SeverityFilter.hpp"
#include "reporter/RateLimitFilter.hpp"

#include "reporter/DumpProvider.hpp"

#include "reporter/ErrorDispatcher.hpp"

#include "exceptions/ExceptionHandler.hpp"

#include "reporter/ErrorRecord.hpp"

#include "storage/Database.hpp"

#include "storage/ConfigRepository.hpp"
#include "storage/EventRepository.hpp"

#include "logging/Logger.hpp"

#include "core/DriverConnector.hpp"
#include "core/DriverService.hpp"
#include "core/EventProcessor.hpp"

#include "exceptions/SysMonException.hpp"

#include "gui/DX11Renderer.hpp"
#include "gui/UIRenderer.hpp"
#include "gui/PageManager.hpp"
#include "gui/Router.hpp"

#include "gui/pages/Dashboard.hpp"
#include "gui/pages/Processes.hpp"
#include "gui/pages/Threads.hpp"
#include "gui/pages/Images.hpp"
#include "gui/pages/Settings.hpp"

#include "gui/components/Sidebar.hpp"

/**
 * @file Application.cpp
 * @brief Implements Application: DI wiring, main loop and shutdown sequence.
 */

namespace
{
    /** @brief Fills ErrorRecord's static app/system info fields (name, version, host, arch...). */
    void PopulateErrorRecordEnvironment()
    {
        ErrorRecord::AppInfo.Name    = "SysMonClient";
#ifdef SYSMON_VERSION
        ErrorRecord::AppInfo.Version = SYSMON_VERSION;
#endif
#ifdef _DEBUG
        ErrorRecord::AppInfo.Build   = "Debug";
#else
        ErrorRecord::AppInfo.Build   = "Release";
#endif

        char  hostname[MAX_COMPUTERNAME_LENGTH + 1];
        DWORD hostnameSize = sizeof(hostname);

        if (GetComputerNameA(hostname, &hostnameSize))
            ErrorRecord::SysInfo.HostName = hostname;

        char  username[256];
        DWORD usernameSize = sizeof(username);

        if (GetUserNameA(username, &usernameSize))
            ErrorRecord::SysInfo.Username = username;

        ErrorRecord::SysInfo.Os = "Windows";

        SYSTEM_INFO systemInfo{};
        GetNativeSystemInfo(&systemInfo);

        switch (systemInfo.wProcessorArchitecture)
        {
            case PROCESSOR_ARCHITECTURE_AMD64: ErrorRecord::SysInfo.Arch = "x64";     break;
            case PROCESSOR_ARCHITECTURE_ARM64: ErrorRecord::SysInfo.Arch = "arm64";   break;
            case PROCESSOR_ARCHITECTURE_INTEL: ErrorRecord::SysInfo.Arch = "x86";     break;
            default:                           ErrorRecord::SysInfo.Arch = "unknown"; break;
        }
    }
}

/** @brief Stores the process launch parameters; real initialization happens in Run(). */
Application::Application(HINSTANCE hInstance, int nCmdShow)
    : _Running(false), _Error(0), _HInstance(hInstance), _NCmdShow(nCmdShow)
{
}

/** @brief Initializes the services, runs the main loop and returns the process exit code. */
uint32_t Application::Run() noexcept
{
    Init();
    _Running = true;
    _Container.Resolve<EventProcessor>()->Start();
    LogicLoop();
    return _Error;
}

/** @brief Stops the background services and releases the UI resources. */
void Application::Shutdown() noexcept
{
    _Running = false;
    _Container.Resolve<EventProcessor>()->Stop();
    _Container.Resolve<DriverConnector>()->Disconnect();
    _Container.Resolve<DriverService>()->Stop();
    _Container.Resolve<UIRenderer>()->Shutdown();
    _Window->Shutdown();
    _Container.Resolve<ErrorDispatcher>()->Shutdown();
}

/** @brief Registers every service in the container and starts the driver pipeline. */
void Application::Init()
{
    PopulateErrorRecordEnvironment();
    // reporters
    _Container.Register(std::make_shared<ErrorQueue>());
    _Container.Register(std::make_shared<FileSink>("SysMon", std::filesystem::current_path() / "logs" / "errors.log"));
    _Container.Register(std::make_shared<JsonFormatter>());
    _Container.Register(std::make_shared<RateLimitFilter>(10, 60000));
    _Container.Register(std::make_shared<SeverityFilter>(ErrorSeverity::Info));
    _Container.Register(std::make_shared<DumpProvider>());
    _Container.Register(std::make_shared<ErrorDispatcher>(_Container.Resolve<ErrorQueue>()));
    // exceptions
    _Container.Register(std::make_shared<ExceptionHandler>(
        _Container.Resolve<ErrorDispatcher>(), 
        _Container.Resolve<DumpProvider>())
    );
    // storage
    _Container.Register(std::make_shared<Database>());
    _Container.Register(std::make_shared<ConfigRepository>(_Container.Resolve<Database>()));
    _Container.Register(std::make_shared<EventRepository>(_Container.Resolve<Database>()));
    // logger
    _Container.Register(std::make_shared<Logger>(_Container.Resolve<ErrorDispatcher>()));
    // core
    _Container.Register(std::make_shared<DriverService>(
        L"SysMon",
        _Container.Resolve<Logger>()
    ));
    _Container.Register(std::make_shared<DriverConnector>());
    _Container.Register(std::make_shared<EventProcessor>(
        _Container.Resolve<DriverConnector>(), 
        _Container.Resolve<EventRepository>(), 
        _Container.Resolve<ErrorDispatcher>())
    );
    // gui
    _Container.Register(std::make_shared<PageManager>());
    _Container.Register(std::make_shared<Router>(
        _Container.Resolve<PageManager>()
    ));
    _Container.Register(std::make_shared<DX11Renderer>());
    _Container.Register(std::make_shared<UIRenderer>(
        _Container.Resolve<DX11Renderer>(),
        _Container.Resolve<Router>()
    ));
    // pages
    _Container.Register(std::make_shared<Dashboard>(
        _Container.Resolve<EventRepository>()
    ));
    _Container.Register(std::make_shared<Processes>(
        _Container.Resolve<EventRepository>(),
        _Container.Resolve<ConfigRepository>()
    ));
    _Container.Register(std::make_shared<Threads>(
        _Container.Resolve<EventRepository>(),
        _Container.Resolve<ConfigRepository>()
    ));
    _Container.Register(std::make_shared<Images>(
        _Container.Resolve<EventRepository>(),
        _Container.Resolve<ConfigRepository>()
    ));
    _Container.Register(std::make_shared<Settings>(
        _Container.Resolve<ConfigRepository>(),
        _Container.Resolve<EventRepository>()
    ));
    // components
    _Container.Register(std::make_shared<Sidebar>(
        _Container.Resolve<Router>(),
        _Container.Resolve<PageManager>()
    ));
    // intialize dispatcher
    auto dispatcher = _Container.Resolve<ErrorDispatcher>();
    dispatcher->AddSink(_Container.Resolve<FileSink>());
    dispatcher->AddFilter(_Container.Resolve<RateLimitFilter>());
    dispatcher->AddFilter(_Container.Resolve<SeverityFilter>());
    dispatcher->SetFormatter(_Container.Resolve<JsonFormatter>());
    auto db = _Container.Resolve<Database>();
    db->Open((std::filesystem::current_path() / "sysmon.db").string());
    _Container.Resolve<DriverService>()->Start();
    auto driverConnector  = _Container.Resolve<DriverConnector>();
    driverConnector->Connect(L"\\\\.\\SysMon");
    // window
    _Window = std::make_unique<Win32Window>();
    _Window->Initialize(_HInstance, _NCmdShow);
    _Window->SetResizeCallback([this](UINT w, UINT h) {
        _Container.Resolve<DX11Renderer>()->SetResizeSize(w, h);
    });
    _Container.Resolve<UIRenderer>()->Initialize(_Window->GetHWND());
    // Pages initialization
    auto pageManager = _Container.Resolve<PageManager>();
    auto sidebar     = _Container.Resolve<Sidebar>();

    auto registerPage = [&](const char* id, std::shared_ptr<IPage> page) {
        pageManager->RegisterPage(id, std::move(page));
        sidebar->AddEntry(id);
    };

    registerPage(Dashboard::ClassName(), _Container.Resolve<Dashboard>());
    registerPage(Processes::ClassName(), _Container.Resolve<Processes>());
    registerPage(Threads::ClassName(),   _Container.Resolve<Threads>());
    registerPage(Images::ClassName(),    _Container.Resolve<Images>());
    registerPage(Settings::ClassName(),  _Container.Resolve<Settings>());

    _Container.Resolve<UIRenderer>()->SetSidebar(sidebar);
    // Navigate to the dashboard
    _Container.Resolve<Router>()->Navigate(Dashboard::ClassName());
}

/** @brief Background loop for non-UI work while the window is running. */
void Application::LogicLoop()
{
    while (_Running)
    {
        try
        {
            _Window->ProcessMessages();

            if (!_Window->IsRunning())
            {
                Shutdown();
                break;
            }

            _Container.Resolve<UIRenderer>()->Render();
        }
        catch(const SysMonException& e)
        {
            _Container.Resolve<ExceptionHandler>()->Handle(e);

            if (e.Severity() == ErrorSeverity::Fatal)
            {
                _Error = 1;
                Shutdown();
            }
        }
        catch(...)
        {
            _Container.Resolve<ExceptionHandler>()->HandleUnknown(std::current_exception());
            _Error = 2;
            Shutdown();
        }
    }
}