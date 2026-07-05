#pragma once

/**
 * @file Locker.h
 * @brief RAII lock guards (Locker, SharedLocker) for exception/early-return
 *        safe acquisition and release of exclusive or shared locks.
 */

/**
 * @brief RAII guard that acquires a lock exclusively on construction
 *        and releases it on destruction.
 *
 * @tparam Lock Any type exposing Lock() / Unlock() (e.g. FastMutex, ExecutiveResource)
 */
template <typename Lock>
struct Locker
{
    inline explicit Locker(Lock& lock) : _Lock(lock)
    {
        _Lock.Lock();
    }

    inline ~Locker()
    {
        _Lock.Unlock();
    }

private:
    Lock& _Lock;
};

/**
 * @brief RAII guard that acquires a lock in shared mode on construction
 *        and releases it on destruction.
 *
 * @tparam Lock Any type exposing LockShared() / UnlockShared() (e.g. ExecutiveResource)
 */
template <typename Lock>
struct SharedLocker
{
    inline explicit SharedLocker(Lock& lock) : _Lock(lock)
    {
        _Lock.LockShared();
    }

    inline ~SharedLocker()
    {
        _Lock.UnlockShared();
    }

private:
    Lock& _Lock;
};
