#pragma once

#include "interfaces/IErrorSink.hpp"

#include <string>
#include <ostream>
#include <fstream>

#include <filesystem>

class FileSink : public IErrorSink
{
public:
    FileSink(const std::string& name, std::filesystem::path path);

    ~FileSink() override;

    void Send(std::string_view payload, std::string_view contentType) noexcept override;

    void Flush() noexcept override;
    void Shutdown() noexcept override;

    std::string_view Name() const noexcept override;

private:
    std::string                 _Name;
    std::ofstream               _Stream;
};