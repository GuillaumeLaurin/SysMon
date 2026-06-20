#pragma once

#include "interfaces/IErrorFilter.hpp"

#include <string>
#include <vector>

class CategoryFilter : public IErrorFilter
{
public:
    CategoryFilter(const std::vector<std::string>& blacklist);

    ~CategoryFilter() override = default;

    bool ShouldProcess(const ErrorRecord& record) const noexcept override;

private:
    std::vector<std::string> _Blacklist;
};