#pragma once

#include <string>

#include <memory>

#include <typeinfo>

#include <unordered_map>

class DIContainer
{
public:
    template<typename T>
    void Register(std::shared_ptr<T> instance);

    template<typename T>
    std::shared_ptr<T> Resolve();

private:
    std::unordered_map<std::string, std::shared_ptr<void>> _Instances;
};

template<typename T>
void DIContainer::Register(std::shared_ptr<T> instance)
{
    std::string registar = typeid(T).name();

    if (_Instances.find(registar) == _Instances.end())
    {
        _Instances[registar] = std::static_pointer_cast<void>(instance);
    }
}

template<typename T>
std::shared_ptr<T> DIContainer::Resolve()
{
    return std::static_pointer_cast<T>(_Instances[typeid(T).name()]);
}