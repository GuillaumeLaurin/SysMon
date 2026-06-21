#pragma once

#include "interfaces/IEventProcessor.hpp"

#include "interfaces/IEventRepository.hpp"

#include "interfaces/IDriverConnector.hpp"

#include "interfaces/IErrorDispatcher.hpp"

#include <atomic>
#include <thread>
#include <memory>

class EventProcessor : public IEventProcessor
{
public:
    EventProcessor(
        std::shared_ptr<IDriverConnector> connector,
        std::shared_ptr<IEventRepository> repository,
        std::shared_ptr<IErrorDispatcher> dispatcher
    );

    ~EventProcessor() override;

    void Start() noexcept override;
    void Stop() noexcept override;

private:
    std::shared_ptr<IDriverConnector> _Connector;
    std::shared_ptr<IEventRepository> _Repository;
    std::shared_ptr<IErrorDispatcher> _Dispatcher;
    std::thread                       _Worker;
    std::atomic<bool>                 _Running = false;

    void WorkerLoop();
    void ProcessBuffer(BYTE* buffer, DWORD size);
    std::wstring GetDosNameFromNTName(PCWSTR path);
    std::string WStringToString(const std::wstring& wstr);
};
