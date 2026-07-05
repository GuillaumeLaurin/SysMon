/**
 * @file FileSink.hpp
 * @brief IErrorSink implementation appending formatted payloads to a log file.
 */

#pragma once

#include "interfaces/IErrorSink.hpp"

#include <string>
#include <ostream>
#include <fstream>

#include <filesystem>

/**
 * @brief Sink appending formatted payloads to a log file (one per line).
 */
class FileSink : public IErrorSink
{
public:
    /**
     * @brief Opens the log file in append mode.
     * @param name Sink identifier.
     * @param path Log file path.
     * @note Unlike other sinks in this module, this constructor does not
     *       create the parent directories itself; the caller/DumpProvider-style
     *       callers are expected to ensure the directory exists, otherwise the
     *       underlying std::ofstream silently fails to open.
     */
    FileSink(const std::string& name, std::filesystem::path path);

    /** @brief Closes the stream via Shutdown(). */
    ~FileSink() override;

    /**
     * @brief Appends one payload followed by a newline.
     * @param payload Formatted payload to write.
     * @param contentType Unused; the file sink writes payloads verbatim regardless of MIME type.
     * @note No-op if the underlying stream failed to open.
     */
    void Send(std::string_view payload, std::string_view contentType) noexcept override;

    /**
     * @brief Flushes the stream to disk.
     * @note No-op if the underlying stream failed to open.
     */
    void Flush() noexcept override;

    /** @brief Flushes and closes the stream. */
    void Shutdown() noexcept override;

    /**
     * @brief Sink identifier.
     * @return The name passed at construction.
     */
    std::string_view Name() const noexcept override;

private:
    std::string                 _Name;   ///< Sink identifier used for registration/removal.
    std::ofstream               _Stream; ///< Output file stream opened in append mode.
};
