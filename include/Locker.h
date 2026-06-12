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
    Lock& _lock;
};

template <typename Lock>
struct SharedLocker
{
    inline explicit SharedLocker(Lock& lock) : _Lock(lock)
    {
        lock.LockShared();
    }

    inline ~SharedLocker()
    {
        _Lock.UnlockShared();
    }

private:
    Lock& _Lock;
};