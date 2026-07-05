/**
 * @file IDumpProvider.hpp
 * @brief Contract for the component producing process memory dumps on fatal errors.
 */

#pragma once

#include "reporter/ErrorRecord.hpp"

/**
 * @brief Kind of memory dump to produce.
 */
enum class DumpType
{
    Mini,    ///< Minimal dump (stacks, module list)
    Full,    ///< Full memory dump
    WithHeap ///< Mini dump plus process heaps
};

/**
 * @brief Produces process memory dumps when a fatal error is reported.
 */
class IDumpProvider
{
public:
    /**
     * @brief Virtual destructor allowing safe destruction through the interface pointer.
     */
    virtual ~IDumpProvider() = default;

    /**
     * @brief Writes a memory dump for the current process.
     * @param trigger    Error record that triggered the dump.
     * @param outputPath Destination file path.
     * @param type       Kind of dump to produce.
     * @return true on success, false if the dump could not be written.
     * @note Must not throw; implementations should catch and report failures via the return value.
     */
    virtual bool GenerateDump(
        const ErrorRecord& trigger,
        std::wstring_view  outputPath,
        DumpType           type
    ) noexcept = 0;

    /**
     * @brief Returns true if dump generation is available on this system.
     * @return true if the current platform/permissions allow producing a dump.
     */
    virtual bool IsSupported() const noexcept = 0;
};
