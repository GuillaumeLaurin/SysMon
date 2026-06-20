#include "reporter/ConsoleSink.hpp"

ConsoleSink::ConsoleSink(const std::string& name, FILE* file)
    : _Name(name), _File(file == nullptr || file == stdin ? stdout : file)
{
}

void ConsoleSink::Send(std::string_view payload, std::string_view /*contentType*/) noexcept
{
    fprintf(_File, "[%s] %.*s\n", _Name.c_str(), (int)payload.size(), payload.data());
}

void ConsoleSink::Flush() noexcept
{
    fflush(_File);
}

void ConsoleSink::Shutdown() noexcept
{
    Flush();
}

std::string_view ConsoleSink::Name() const noexcept
{
    return _Name;
}