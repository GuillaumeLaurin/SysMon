#pragma once

class IEventProcessor
{
public:
    virtual ~IEventProcessor() = default;

    virtual void Start() noexcept = 0;
    virtual void Stop() noexcept = 0;
};