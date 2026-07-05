/**
 * @file JsonFormatter.hpp
 * @brief IErrorFormatter implementation serializing an ErrorRecord to a
 *        pretty-printed JSON document.
 */

#pragma once

#include "interfaces/IErrorFormatter.hpp"

#include <nlohmann/json.hpp>

/**
 * @brief Serializes error records to JSON (content type "application/json").
 */
class JsonFormatter : public IErrorFormatter
{
public:
    /** @brief Default destructor. */
    ~JsonFormatter() override = default;

    /**
     * @brief Serializes the full record (app/system info, context, metadata) to JSON.
     * @param record Record to serialize.
     * @return A 4-space-indented JSON document as a string.
     */
    std::string Format(const ErrorRecord& record) const override;

    /**
     * @brief Returns "application/json".
     * @return The MIME type of the produced payload.
     */
    std::string_view ContentType() const noexcept override;

private:
    /**
     * @brief Maps an ErrorSeverity to its string representation.
     * @param severity Severity to convert.
     * @return "Debug", "Info", "Warning", "Error", "Fatal", or "Unknown" for an unrecognized value.
     */
    static std::string SeverityToString(ErrorSeverity severity);

    /**
     * @brief Converts a Windows FILETIME-based timestamp to an ISO-8601 string.
     * @param timestamp Timestamp expressed as a FILETIME-compatible LARGE_INTEGER (UTC).
     * @return ISO-8601 string with millisecond precision and a "Z" (UTC) suffix,
     *         or "1970-01-01T00:00:00.000Z" if the conversion fails.
     */
    static std::string TimestampToIso8601(LARGE_INTEGER timestamp);

    /**
     * @brief Formats an address as 0x-prefixed hexadecimal.
     * @param address Address value to format.
     * @return Zero-padded, uppercase hexadecimal string prefixed with "0x".
     */
    static std::string AddressToHexString(ULONG_PTR address);

    /**
     * @brief Converts the metadata pairs into a JSON array of {key, value} objects.
     * @param params Metadata key/value pairs to convert.
     * @return JSON array where each element is an object with "key" and "value" fields.
     */
    static nlohmann::json MetadataToJsonArray(const SysMon::Params& params);
};
