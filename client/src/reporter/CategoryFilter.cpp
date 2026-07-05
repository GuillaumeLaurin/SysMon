/**
 * @file CategoryFilter.cpp
 * @brief Implementation of CategoryFilter.
 */

#include "reporter/CategoryFilter.hpp"

#include <algorithm>

/** @brief Copies the blacklist into the filter. */
CategoryFilter::CategoryFilter(const std::vector<std::string>& blacklist)
    : _Blacklist(blacklist)
{
}

/** @brief Linear-searches the blacklist for the record's category. */
bool CategoryFilter::ShouldProcess(const ErrorRecord& record) const noexcept
{
    auto found = std::find_if(_Blacklist.begin(), _Blacklist.end(), 
        [&](const std::string& blacklised) { return record.Category == blacklised; });

    return found == _Blacklist.end();
}