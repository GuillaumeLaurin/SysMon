/**
 * @file CategoryFilter.hpp
 * @brief IErrorFilter implementation that blocks error records whose category
 *        appears in a fixed blacklist.
 */

#pragma once

#include "interfaces/IErrorFilter.hpp"

#include <string>
#include <vector>

/**
 * @brief Filter dropping records whose category belongs to a blacklist.
 */
class CategoryFilter : public IErrorFilter
{
public:
    /**
     * @brief Constructs the filter with a fixed list of blocked categories.
     * @param blacklist Categories to drop.
     */
    CategoryFilter(const std::vector<std::string>& blacklist);

    /** @brief Default destructor. */
    ~CategoryFilter() override = default;

    /**
     * @brief Returns false when the record's category is blacklisted.
     * @param record Record to evaluate.
     * @return true if the record's Category is not in the blacklist.
     */
    bool ShouldProcess(const ErrorRecord& record) const noexcept override;

private:
    std::vector<std::string> _Blacklist; ///< Categories that must be dropped.
};
