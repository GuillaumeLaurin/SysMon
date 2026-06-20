#include "reporter/FileSink.hpp"

FileSink::FileSink(const std::string& name, std::filesystem::path path)
    : _Name(name), _Stream(path, std::ios::app)
{
}

FileSink::~FileSink()
{
    Shutdown();
}

void FileSink::Send(std::string_view payload, std::string_view /*contentType*/) noexcept
{
    if (!_Stream.is_open())
    {
        return;
    }

    _Stream << "[" << _Name << "]" << " " << payload << "\n";
    Flush();
}

void FileSink::Flush() noexcept
{
    if (!_Stream.is_open())
    {
        return;
    }

    _Stream << std::flush;
}

void FileSink::Shutdown() noexcept
{
    Flush();
    _Stream.close();
}

std::string_view FileSink::Name() const noexcept
{
    return _Name;
}