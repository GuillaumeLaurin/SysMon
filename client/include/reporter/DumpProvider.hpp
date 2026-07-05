/**
 * @file DumpProvider.hpp
 * @brief IDumpProvider implementation producing process minidumps via DbgHelp.
 */

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include <windows.h>

#include "interfaces/IDumpProvider.hpp"

/**
 * @brief IDumpProvider implementation based on DbgHelp MiniDumpWriteDump.
 */
class DumpProvider : public IDumpProvider
{
public:
    /** @brief Captures a handle to the current process for later dump generation. */
    DumpProvider();

    /** @brief Default destructor. */
    ~DumpProvider() override = default;

    /**
     * @brief Writes a minidump of the current process to @p outputPath.
     * @param trigger Error record that triggered the dump; its Fingerprint is
     *                used to name the dump file ("<fingerprint>.dmp").
     * @param outputPath Destination directory; created if it does not exist.
     * @param type Kind of dump to produce (Mini, Full, or WithHeap).
     * @return true if the dump file was written successfully; false if the
     *         file could not be created or MiniDumpWriteDump() failed.
     */
    bool GenerateDump(
        const ErrorRecord& trigger,
        std::wstring_view  outputPath,
        DumpType           type
    ) noexcept override;

    /**
     * @brief Returns true when DbgHelp dump generation is available.
     * @return Always true in this implementation.
     */
    bool IsSupported() const noexcept override;

private:
    HANDLE _CurrentProcess; ///< Pseudo-handle to the current process, cached at construction.
};
