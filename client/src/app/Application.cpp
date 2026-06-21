#include "app/Application.hpp"

#include "reporter/ErrorQueue.hpp"

#include "reporter/FileSink.hpp"
#include "reporter/ConsoleSink.hpp"

#include "reporter/JsonFormatter.hpp"

#include "reporter/SeverityFilter.hpp"
#include "reporter/RateLimitFilter.hpp"

#include "reporter/DumpProvider.hpp"

#include "reporter/ErrorDispatcher.hpp"

#include "exceptions/ExceptionHandler.hpp"

#include "storage/Database.hpp"

#include "storage/ConfigRepository.hpp"
#include "storage/EventRepository.hpp"

#include "logging/Logger.hpp"

#include "core/DriverConnector.hpp"
#include "core/EventProcessor.hpp"

#include "exceptions/SysMonException.hpp"

Application::Application()
    : _Running(false), _Error(0)
{
}

uint32_t Application::Run() noexcept
{
    Init();
    _Running = true;
    _Container.Resolve<EventProcessor>()->Start();
    LogicLoop();
    return _Error;
}

void Application::Shutdown() noexcept
{
    _Running = false;
    _Container.Resolve<EventProcessor>()->Stop();
    _Container.Resolve<ErrorDispatcher>()->Shutdown();
}

void Application::Init()
{
    // reporters
    _Container.Register(std::make_shared<ErrorQueue>());
    _Container.Register(std::make_shared<FileSink>("SysMon", std::filesystem::current_path() / "logs" / "errors.log"));
    _Container.Register(std::make_shared<ConsoleSink>("SysMonConsole", stderr));
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
    _Container.Register(std::make_shared<DriverConnector>());
    _Container.Register(std::make_shared<EventProcessor>(
        _Container.Resolve<DriverConnector>(), 
        _Container.Resolve<EventRepository>(), 
        _Container.Resolve<ErrorDispatcher>())
    );
    // intialize dispatcher
    auto dispatcher = _Container.Resolve<ErrorDispatcher>();
    dispatcher->AddSink(_Container.Resolve<FileSink>());
    dispatcher->AddSink(_Container.Resolve<ConsoleSink>());
    dispatcher->AddFilter(_Container.Resolve<RateLimitFilter>());
    dispatcher->AddFilter(_Container.Resolve<SeverityFilter>());
    dispatcher->SetFormatter(_Container.Resolve<JsonFormatter>());
    auto db = _Container.Resolve<Database>();
    db->Open((std::filesystem::current_path() / "sysmon.db").string());
    auto driverConnector  = _Container.Resolve<DriverConnector>();
    driverConnector->Connect(L"\\\\.\\SysMon");
}

void Application::LogicLoop()
{
    while (_Running)
    {
        try
        {
            Sleep(100);
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