/**
 * @file FileSink.cpp
 * @brief Implementation of FileSink.
 */

#include "reporter/FileSink.hpp"

/** @brief Opens the file stream in append mode. */
FileSink::FileSink(const std::string& name, std::filesystem::path path)
    : _Name(name), _Stream(path, std::ios::app)
{
}

/** @brief Shuts the sink down, flushing and closing the stream. */
FileSink::~FileSink()
{
    Shutdown();
}

/** @brief Writes "[name] payload\n" to the stream and flushes, if the stream is open. */
void FileSink::Send(std::string_view payload, std::string_view /*contentType*/) noexcept
{
    if (!_Stream.is_open())
    {
        return;
    }

    _Stream << "[" << _Name << "]" << " " << payload << "\n";
    Flush();
}

/** @brief Flushes the stream, if open. */
void FileSink::Flush() noexcept
{
    if (!_Stream.is_open())
    {
        return;
    }

    _Stream << std::flush;
}

/** @brief Flushes then closes the stream. */
void FileSink::Shutdown() noexcept
{
    Flush();
    _Stream.close();
}

/** @brief Returns the configured sink name. */
std::string_view FileSink::Name() const noexcept
{
    return _Name;
}