#pragma once

#include "reporter/ErrorRecord.hpp"

enum class DumpType
{
    Mini,
    Full,
    WithHeap
};

class IDumpProvider
{
public:
    virtual ~IDumpProvider() = default;

    virtual bool GenerateDump(
        const ErrorRecord& trigger,
        std::wstring_view  outputPath,
        DumpType           type
    ) noexcept = 0;

    virtual bool IsSupported() const noexcept = 0;
};