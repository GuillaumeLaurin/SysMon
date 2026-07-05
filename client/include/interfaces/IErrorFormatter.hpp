/**
 * @file IErrorFormatter.hpp
 * @brief Contract for serializing error records into a sink-consumable payload.
 */

#pragma once

#include "reporter/ErrorRecord.hpp"

/**
 * @brief Serializes error records into a payload consumable by the sinks.
 */
class IErrorFormatter
{
public:
    /**
     * @brief Virtual destructor allowing safe destruction through the interface pointer.
     */
    virtual ~IErrorFormatter() = default;

    /**
     * @brief Serializes the record (e.g. to JSON).
     * @param record Error record to serialize.
     * @return The serialized payload as a string.
     */
    virtual std::string Format(const ErrorRecord& record) const = 0;

    /**
     * @brief MIME type of the produced payload (e.g. "application/json").
     * @return The MIME type describing the payload produced by Format().
     */
    virtual std::string_view ContentType() const noexcept = 0;
};
