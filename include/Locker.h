#pragma once

template <typename Lock>
struct Locker
{
    inline explicit Locker(Lock& lock) : _lock(lock)
    {
        _lock.Lock();
    }

    inline ~Locker()
    {
        _lock.Unlock();
    }

private:
    Lock _lock;
};