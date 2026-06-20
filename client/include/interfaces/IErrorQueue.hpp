#pragma once

#include "reporter/ErrorRecord.hpp"

class IErrorQueue
{
public:
    virtual ~IErrorQueue() = default;

    virtual void Enqueue(ErrorRecord record) noexcept = 0;

    virtual bool Dequeue(ErrorRecord& out, DWORD timeoutMs) noexcept = 0;

    virtual size_t Size() const noexcept = 0;
    virtual bool Empty() const noexcept = 0;

    virtual void Clear() noexcept = 0;

    virtual void Shutdown() noexcept = 0;
};