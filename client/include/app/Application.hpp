#pragma once

#include "core/DIContainer.hpp"

#include <memory>
#include <atomic>

class Application
{
public:
    Application();

    uint32_t Run() noexcept;
    void Shutdown() noexcept;

private:
    DIContainer                  _Container;
    std::atomic<bool>            _Running;
    uint32_t                     _Error;

    void LogicLoop();
    void Init();
};