#pragma once

#define SYSMON_CLASS(cls) \
    public: \
        static constexpr const char* ClassName() { return #cls; }