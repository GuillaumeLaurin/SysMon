/**
 * @file IErrorSink.hpp
 * @brief Contract for destinations of formatted error payloads.
 */

#pragma once

#include <string>

/**
 * @brief Destination for formatted error payloads (console, log file, ...).
 */
class IErrorSink
{
public:
    /**
     * @brief Virtual destructor allowing safe destruction through the interface pointer.
     * @note Implementations should call Shutdown() if not already done.
     */
    virtual ~IErrorSink() = default;

    /**
     * @brief Writes one formatted payload to the destination.
     * @param payload Serialized payload produced by an IErrorFormatter.
     * @param contentType MIME type describing @p payload.
     */
    virtual void Send(std::string_view payload, std::string_view contentType) noexcept = 0;

    /**
     * @brief Forces any buffered output to be written.
     */
    virtual void Flush() noexcept = 0;

    /**
     * @brief Releases resources; no Send() may follow.
     */
    virtual void Shutdown() noexcept = 0;

    /**
     * @brief Unique sink name, used for removal.
     * @return The sink's unique name, as passed to IErrorDispatcher::RemoveSink().
     */
    virtual std::string_view Name() const noexcept = 0;
};
