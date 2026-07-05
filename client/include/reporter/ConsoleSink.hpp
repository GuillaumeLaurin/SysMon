/**
 * @file ConsoleSink.hpp
 * @brief IErrorSink implementation writing formatted payloads to a C stream.
 */

#pragma once

#include "interfaces/IErrorSink.hpp"

#include <string>

/**
 * @brief Sink writing formatted payloads to a C stream (stdout by default).
 */
class ConsoleSink : public IErrorSink
{
public:
    /** @brief Default destructor; the underlying stream is not owned/closed. */
    ~ConsoleSink() override = default;

    /**
     * @brief Constructs the sink.
     * @param name Sink identifier.
     * @param file Target stream; if null or stdin, defaults to stdout.
     */
    ConsoleSink(const std::string& name, FILE* file = nullptr);

    /**
     * @brief Writes one payload followed by a newline.
     * @param payload Formatted payload to write.
     * @param contentType Unused; the console sink writes payloads verbatim regardless of MIME type.
     */
    void Send(std::string_view payload, std::string_view contentType) noexcept override;

    /** @brief Flushes the underlying stream. */
    void Flush() noexcept override;

    /**
     * @brief Flushes; the stream itself is not owned so it is not closed.
     */
    void Shutdown() noexcept override;

    /**
     * @brief Sink identifier.
     * @return The name passed at construction.
     */
    std::string_view Name() const noexcept override;

private:
    std::string _Name;       ///< Sink identifier used for registration/removal.
    FILE* const _File;       ///< Target C stream (never owned/closed by this sink).
};
