#pragma once

#include "interfaces/IErrorFormatter.hpp"

#include <nlohmann/json.hpp>

class JsonFormatter : public IErrorFormatter
{
public:
    ~JsonFormatter() override = default;

    std::string Format(const ErrorRecord& record) const override;

    std::string_view ContentType() const noexcept override;

private:
    static std::string SeverityToString(ErrorSeverity severity);

    static std::string TimestampToIso8601(LARGE_INTEGER timestamp);

    static std::string AddressToHexString(ULONG_PTR address);

    static nlohmann::json MetadataToJsonArray(const SysMon::Params& params);
};

