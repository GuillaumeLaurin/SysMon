/**
 * @file ConsoleSink.cpp
 * @brief Implementation of ConsoleSink.
 */

#include "reporter/ConsoleSink.hpp"

/** @brief Stores the name; falls back to stdout when @p file is null or stdin. */
ConsoleSink::ConsoleSink(const std::string& name, FILE* file)
    : _Name(name), _File(file == nullptr || file == stdin ? stdout : file)
{
}

/** @brief Writes "[name] payload\n" to the target stream via fprintf. */
void ConsoleSink::Send(std::string_view payload, std::string_view /*contentType*/) noexcept
{
    fprintf(_File, "[%s] %.*s\n", _Name.c_str(), (int)payload.size(), payload.data());
}

/** @brief Calls fflush() on the target stream. */
void ConsoleSink::Flush() noexcept
{
    fflush(_File);
}

/** @brief Flushes the stream; the stream is not closed since it is not owned. */
void ConsoleSink::Shutdown() noexcept
{
    Flush();
}

/** @brief Returns the configured sink name. */
std::string_view ConsoleSink::Name() const noexcept
{
    return _Name;
}