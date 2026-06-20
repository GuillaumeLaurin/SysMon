#include "reporter/ErrorRecord.hpp"

#include <sstream>

#include <iomanip>

SysMon::ApplicationInformation ErrorRecord::AppInfo{};

SysMon::SystemInformation ErrorRecord::SysInfo{};

uint64_t ErrorRecord::Fnv1a(std::string_view data, uint64_t hash)
{
    constexpr uint64_t FNV_PRIME = 0x100000001B3;

    for (unsigned char byte : data)
    {
        hash ^= byte;
        hash *= FNV_PRIME;
    }

    return hash;
}

std::string ErrorRecord::ComputeFingerprint(const ErrorRecord& report)
{
    constexpr uint64_t FNV_OFFSET_BASIS = 0xCBF29CE484222325;

    uint64_t hash = FNV_OFFSET_BASIS;
    hash = Fnv1a(report.Context.File, hash);
    hash = Fnv1a(report.Context.Function, hash);
    hash = Fnv1a(report.Context.Stacktrace.substr(0, 100), hash);
    hash = Fnv1a(std::to_string(report.Context.Line), hash);

    std::ostringstream oss;
    oss << std::hex << std::setw(16) << std::setfill('0') << hash;
    return oss.str();
}