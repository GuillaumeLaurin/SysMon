#pragma once

#include "interfaces/IErrorSink.hpp"

#include <string>

class ConsoleSink : public IErrorSink
{
public:
    ~ConsoleSink() override = default;

    ConsoleSink(const std::string& name, FILE* file = nullptr);

    void Send(std::string_view payload, std::string_view contentType) noexcept override;

    void Flush() noexcept override;
    void Shutdown() noexcept override;

    std::string_view Name() const noexcept override;

private:
    std::string _Name;
    FILE* const _File;
};