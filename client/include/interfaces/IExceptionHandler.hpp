/**
 * @file IExceptionHandler.hpp
 * @brief Contract for the top-level handler converting exceptions into error records.
 */

#pragma once

#include "exceptions/SysMonException.hpp"

#include "interfaces/IErrorDispatcher.hpp"

#include <memory>

/**
 * @brief Top-level handler converting exceptions into error records
 *        and forwarding them to the error dispatcher.
 *
 * @note Implementations must not throw: Handle()/HandleUnknown() are typically
 *       invoked from catch blocks or terminate handlers where propagating a new
 *       exception is unsafe.
 */
class IExceptionHandler
{
public:
    /**
     * @brief Virtual destructor allowing safe destruction through the interface pointer.
     */
    virtual ~IExceptionHandler() = default;

    /**
     * @brief Reports a typed SysMon exception.
     * @param exception SysMon exception (see exceptions/SysMonException.hpp and its
     *        derived types such as ConfigException, DeviceException, StorageException)
     *        to convert into an error record.
     */
    virtual void Handle(const SysMonException& exception) noexcept = 0;

    /**
     * @brief Reports an unknown/foreign exception captured via std::current_exception().
     * @param ptr Captured exception pointer; may wrap any exception type, including
     *        non-SysMon ones (e.g. std::exception, third-party exceptions).
     */
    virtual void HandleUnknown(std::exception_ptr ptr) noexcept = 0;

    /**
     * @brief Dispatcher used to emit the resulting error records.
     * @return Shared pointer to the error dispatcher used internally by this handler.
     */
    virtual std::shared_ptr<IErrorDispatcher> Dispatcher() const noexcept  = 0;
};
