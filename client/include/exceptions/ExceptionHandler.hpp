#pragma once

#include "interfaces/IExceptionHandler.hpp"

#include "interfaces/IErrorDispatcher.hpp"

#include "interfaces/IDumpProvider.hpp"

#include <memory>

/**
 * @file ExceptionHandler.hpp
 * @brief Declares ExceptionHandler, the IExceptionHandler implementation
 *        that converts exceptions into ErrorRecords, dispatches them and
 *        triggers memory dumps on fatal errors.
 */

/**
 * @brief IExceptionHandler implementation: converts exceptions to error
 *        records, dispatches them, and triggers a memory dump on fatal errors.
 */
class ExceptionHandler : public IExceptionHandler
{
public:
    /**
     * @brief Constructs the exception handler with its dependencies.
     * @param dispatcher Dispatcher used to emit the resulting error records.
     * @param dumpProvider Provider used to generate memory dumps on fatal errors.
     */
    ExceptionHandler(
        std::shared_ptr<IErrorDispatcher> dispatcher,
        std::shared_ptr<IDumpProvider> dumpProvider
    );

    /** @brief Default destructor. */
    ~ExceptionHandler() override = default;

    /**
     * @brief Reports a typed SysMon exception; fatal ones also produce a dump.
     * @param exception Exception to report.
     */
    void Handle(const SysMonException& exception) noexcept override;

    /**
     * @brief Reports an unknown/foreign exception captured via std::current_exception().
     * @param ptr Captured exception pointer to rethrow and classify.
     */
    void HandleUnknown(std::exception_ptr ptr) noexcept override;

    /**
     * @brief Sets the directory/file path used for generated dumps.
     * @param outputPath New output path for generated dumps.
     */
    void SetOutputPath(const std::wstring& outputPath) noexcept;

    /**
     * @brief Sets the kind of dump produced on fatal errors.
     * @param dumpType New dump type to use.
     */
    void SetDumpType(DumpType dumpType) noexcept;

    /**
     * @brief Dispatcher used to emit the resulting error records.
     * @return The error dispatcher used by this handler.
     */
    std::shared_ptr<IErrorDispatcher> Dispatcher() const noexcept override;

private:
    /** @brief Dispatcher used to emit the resulting error records. */
    std::shared_ptr<IErrorDispatcher> _ErrorDispatcher;
    /** @brief Provider used to generate memory dumps on fatal errors. */
    std::shared_ptr<IDumpProvider>    _DumpProvider;
    /** @brief Directory/file path used for generated dumps. */
    std::wstring                      _OutputPath;
    /** @brief Kind of dump produced on fatal errors. */
    DumpType                          _DumpType;

    /**
     * @brief Builds an ErrorRecord for exceptions that carry no SysMon metadata.
     * @param category Error category to assign to the record.
     * @param message Error message to assign to the record.
     * @param type Type name of the originating exception.
     * @return A minimally populated ErrorRecord.
     */
    ErrorRecord BuildMinimalRecord(
        std::string category,
        std::string message,
        std::string type
    ) const noexcept;
};
