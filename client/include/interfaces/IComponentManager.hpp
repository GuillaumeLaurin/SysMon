/**
 * @file IComponentManager.hpp
 * @brief Contract for the registry that owns and resolves shared UI components.
 */

#pragma once

#include <memory>
#include <string>

#include "interfaces/IComponent.hpp"

/**
 * @brief Registry of shared UI components addressable by identifier.
 */
class IComponentManager
{
public:
    /**
     * @brief Virtual destructor allowing safe destruction through the interface pointer.
     */
    virtual ~IComponentManager() = default;

    /**
     * @brief Registers a component under the given identifier.
     * @param id Unique identifier used to retrieve the component later.
     * @param component Shared ownership of the component instance to register.
     * @note Registering under an already-used @p id is expected to replace the previous entry.
     */
    virtual void RegisterComponent(const std::string& id, std::shared_ptr<IComponent> component) = 0;

    /**
     * @brief Returns the component registered under @p id, or nullptr if unknown.
     * @param id Identifier used at registration time.
     * @return Shared pointer to the component, or nullptr when no component is registered under @p id.
     */
    virtual std::shared_ptr<IComponent> GetComponent(const std::string& id) const = 0;
};
