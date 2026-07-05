#pragma once

#include <string>

#include <memory>

#include <typeinfo>

#include <unordered_map>

/**
 * @file DIContainer.hpp
 * @brief Declares DIContainer, the minimal type-keyed dependency-injection
 *        container used to wire the application's services.
 */

/**
 * @brief Minimal dependency-injection container storing one shared instance
 *        per type. Instances are registered once and resolved by type.
 */
class DIContainer
{
public:
    /**
     * @brief Registers @p instance as the singleton for type T (first registration wins).
     * @tparam T Type under which the instance is registered.
     * @param instance Shared instance to register.
     */
    template<typename T>
    void Register(std::shared_ptr<T> instance);

    /**
     * @brief Returns the instance registered for type T, or nullptr if none.
     * @tparam T Type of the instance to resolve.
     * @return The registered shared instance, or nullptr if type T was never registered.
     */
    template<typename T>
    std::shared_ptr<T> Resolve();

private:
    /** @brief Registered instances keyed by the mangled type name. */
    std::unordered_map<std::string, std::shared_ptr<void>> _Instances;
};

/** @brief Registers @p instance as the singleton for type T (first registration wins). */
template<typename T>
void DIContainer::Register(std::shared_ptr<T> instance)
{
    std::string key = typeid(T).name();

    if (_Instances.find(key) == _Instances.end())
    {
        _Instances[key] = std::static_pointer_cast<void>(instance);
    }
}

/** @brief Returns the instance registered for type T, or nullptr if none. */
template<typename T>
std::shared_ptr<T> DIContainer::Resolve()
{
    auto it = _Instances.find(typeid(T).name());

    if (it == _Instances.end())
    {
        return nullptr;
    }

    return std::static_pointer_cast<T>(it->second);
}
