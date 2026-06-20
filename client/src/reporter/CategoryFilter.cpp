#include "reporter/CategoryFilter.hpp"

#include <algorithm>

CategoryFilter::CategoryFilter(const std::vector<std::string>& blacklist)
    : _Blacklist(blacklist)
{
}

bool CategoryFilter::ShouldProcess(const ErrorRecord& record) const noexcept
{
    auto found = std::find_if(_Blacklist.begin(), _Blacklist.end(), 
        [&](const std::string& blacklised) { return record.Category == blacklised; });

    return found == _Blacklist.end();
}