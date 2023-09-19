#ifndef __OBJECT_POOL_H__
#define __OBJECT_POOL_H__

#include <assert.h>
#include <stdio.h>
#include <sys/timeb.h>
#include <windows.h>
#include <algorithm>
#include <iostream>
#include <queue>

#include "ErasePtr.h"
#include "ThreadLocal.h"

typedef unsigned long long ULL;

struct TypeDescriptor
{
    void *(*create)();
    void (*destroy)(void *);
};

namespace object_pool
{
    namespace disabled
    {
        void *Get(const TypeDescriptor &desc)
        {
            void *rc = desc.create();
            return rc;
        }

        void Put(const TypeDescriptor &desc, void *ptr)
        {
            desc.destroy(ptr);
        }
    }  // namespace disabled
}  // namespace object_pool

// @sa https://github.com/Tencent/flare
enum PoolType
{
    // Do not use object pool at all.
    //
    // This type is normally used for debugging purpose.
    Disabled,

    // Cache objects in a thread local cache.
    //
    // This type has the highest performance if your object allocation /
    // deallocation is done evenly in every thread.
    //
    // No lock / synchronization is required for this type of pool
    ThreadLocal,
};

template <class T>
struct PoolTraits
{
    // Quiet period before an object is eligible for removal (when there are more
    // than `kLowWaterMark` idle objects are cached). Note that the implementation
    // may delay object deallocation longer than this option.
    // static const size_t kLowWaterMark = ...;

    // This parameter also affects fresh objects. Objects are freed if there are
    // more than `kHighWaterMark` objects alive regardless of their freshness.
    // static const size_t kHighWaterMark = ...;

    // Quiet period before an object is eligible for removal (when there are more
    // than `kLowWaterMark` idle objects are cached). Note that the implementation
    // may delay object deallocation longer than this option.
    // static const size_t kMaxIdle = ...; // millsecond
};

namespace object_pool
{
    namespace detail
    {
        template <class T>
        void *CreatorObject()
        {
            return new T();
        }

        template <class T>
        void DestoryObject(void *ptr)
        {
            T *p = static_cast<T *>(ptr);
            delete p;
        }

        template <class T>
        const TypeDescriptor *GetTypeDesc()
        {
            static const TypeDescriptor desc = {&CreatorObject<T>, &DestoryObject<T>};
            return &desc;
        }

        template <class T>
        inline void *GetWithoutHook()
        {
            const PoolType kType = PoolTraits<T>::kType;
            if (kType == PoolType::Disabled)
            {
                return disabled::Get(*GetTypeDesc<T>());
            }
            else if (kType == PoolType::ThreadLocal)
            {
                return tls::Get(*GetTypeDesc<T>(), tls::GetThreadLocalPool<T>());
            }
            else
            {
                // Unexpected pool type
                abort();
            }
        }

        template <class T>
        inline void PutWithoutHook(void *ptr)
        {
            const PoolType kType = PoolTraits<T>::kType;
            if (kType == PoolType::Disabled)
            {
                disabled::Put(*GetTypeDesc<T>(), ptr);
            }
            else if (kType == PoolType::ThreadLocal)
            {
                tls::Put(*GetTypeDesc<T>(), tls::GetThreadLocalPool<T>(), ptr);
            }
            else
            {
                // Unexpected pool type
                abort();
            }
        }

        template <class T>
        inline void *Get()
        {
            void *rc = GetWithoutHook<T>();
            return rc;
        }

        template <class T>
        inline void Put(void *ptr)
        {
            assert(ptr != NULL);
            return PutWithoutHook<T>(ptr);
        }
    }  // namespace detail
}  // namespace object_pool

// Wrapper for Object
template <class T>
class PooledPtr
{
public:
    PooledPtr();
    ~PooledPtr();

    explicit PooledPtr(T *ptr) : ptr_(ptr) {}

    PooledPtr(const PooledPtr &other) : ptr_(other.ptr_) { const_cast<PooledPtr &>(other).ptr_ = NULL; }

    // Maybe use std::move in C++11
    PooledPtr &operator=(const PooledPtr &other)
    {
        if (this != &other)
        {
            Reset(NULL);
        }
        ptr_ = other.ptr_;
        const_cast<PooledPtr &>(other).ptr_ = NULL;
        return *this;
    }

    void *Leak()
    {
        void *leak = ptr_;
        ptr_ = NULL;
        return leak;
    }

    const T *Get() const;
    const T *operator->() const;
    const T &operator*() const;

    const void Reset(T *ptr = NULL);

private:
    T *ptr_;
};

namespace object_pool
{
    template <class T>
    PooledPtr<T> Get()
    {
        return PooledPtr<T>(static_cast<T *>(detail::Get<T>()));
    }

    template <class T>
    void Put(T *ptr)
    {
        return detail::Put<T>(ptr);
    }
}  // namespace object_pool

template <class T>
PooledPtr<T>::PooledPtr() : ptr_(NULL)
{
}

template <class T>
PooledPtr<T>::~PooledPtr()
{
    if (ptr_)
    {
        object_pool::Put<T>(ptr_);
    }
}

template <class T>
const T *PooledPtr<T>::Get() const
{
    return ptr_;
}

template <class T>
const T *PooledPtr<T>::operator->() const
{
    return ptr_;
}

template <class T>
const T &PooledPtr<T>::operator*() const
{
    return *ptr_;
}

template <class T>
const void PooledPtr<T>::Reset(T *ptr)
{
    if (ptr_)
    {
        object_pool::Put<T>(ptr_);
    }
    ptr_ = ptr;
}

namespace object_pool
{
    namespace tls
    {
        struct TimestampedObject
        {
            ErasedPtr ptr;
            ULL last_used;
            TimestampedObject(void *p, void (*destroy)(void *), ULL lu) : ptr(ErasedPtr(p, destroy)), last_used(lu) {}
            TimestampedObject(const TimestampedObject &other)
            {
                ptr = other.ptr;
                last_used = other.last_used;
            }
        };

        const int kMinimumFreePerWash = 32;
        const int kMinimumWashInterval = 5;  // ms

        // @sa https://blog.csdn.net/Timeinsist/article/details/115456182
        ULL GetCurrentTimestamp()
        {
            FILETIME file_time;
            GetSystemTimeAsFileTime(&file_time);
            ULL time = ((ULL) file_time.dwLowDateTime) + ((ULL) file_time.dwHighDateTime << 32);

            // This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
            // until 00:00:00 January 1, 1970
            static const ULL EPOCH = ((ULL) 116444736000000000ULL);

            return (ULL) ((time - EPOCH) / 10000LL);
        }

        struct PoolDescriptor
        {
            size_t low_water_mark;
            size_t high_water_mark;
            ULL max_idle;
            ULL last_wash;
            std::deque<TimestampedObject> primary_cache;
            std::deque<TimestampedObject> secondary_cache;

            PoolDescriptor() : last_wash(GetCurrentTimestamp()) {}
        };

        template <class T>
        PoolDescriptor *GetThreadLocalPool()
        {
         #if _DEBUG
            assert(PoolTraits<T>::kHighWaterMark > PoolTraits<T>::kLowWaterMark);
         #endif

            static const int kEffectiveHighWaterMark = PoolTraits<T>::kHighWaterMark - PoolTraits<T>::kLowWaterMark;

            static base::ThreadLocal<PoolDescriptor> pool;
            static _declspec(thread) bool initialized = false;
            if(!initialized)
            {
                pool.value().low_water_mark = PoolTraits<T>::kLowWaterMark;
                pool.value().high_water_mark = kEffectiveHighWaterMark;
                pool.value().max_idle = PoolTraits<T>::kMaxIdle;
                initialized = true;
            }
            return &pool.value();
        }

        size_t GetFreeCount(size_t upto)
        {
            return min(upto, max(upto / 2, kMinimumFreePerWash));
        }

        void MoveToSecondaryOrFree(PoolDescriptor *pool, size_t count)
        {
            while (count--)
            {
                assert(pool->primary_cache.size() > 0);
                if (pool->secondary_cache.size() < pool->low_water_mark)
                {
                    pool->secondary_cache.push_back(pool->primary_cache.front());
                }
                pool->primary_cache.pop_front();
            }
        }

        class IdleObjectChecker
        {
        public:
            ULL now;
            ULL maxIdle;

            IdleObjectChecker(ULL currentTime, ULL maxIdleTime) : now(currentTime), maxIdle(maxIdleTime) {}

            bool operator()(const TimestampedObject &obj) const { return now - obj.last_used < maxIdle; }
        };

        void WashOutCache(PoolDescriptor *pool)
        {
            ULL now = GetCurrentTimestamp();
            if (now < pool->last_wash + kMinimumWashInterval)
            {
                // Called too frequently
                return;
            }
            else
            {
                // Update wash out time
                pool->last_wash = now;
            }

            std::deque<TimestampedObject> &primary = pool->primary_cache;
            std::deque<TimestampedObject> &secondary = pool->secondary_cache;

            if (pool->primary_cache.size() > pool->high_water_mark)
            {
                // Too many object in pool
                size_t upto = GetFreeCount(pool->primary_cache.size() - pool->high_water_mark);
                MoveToSecondaryOrFree(pool, upto);
                if (upto == kMinimumFreePerWash)
                {
                    // We've freed enough objects then
                    return;
                }
            }

            IdleObjectChecker checker(now, pool->max_idle);
            size_t idle_objects = std::find_if(primary.begin(), primary.end(), checker) - primary.begin();
            MoveToSecondaryOrFree(pool, GetFreeCount(idle_objects));
        }

        void *Get(const TypeDescriptor &desc, PoolDescriptor *pool)
        {
            if (pool->primary_cache.empty())
            {
                if (!pool->secondary_cache.empty())
                {
                    // Upgrade
                    pool->primary_cache.swap(pool->secondary_cache);

                    for (int i = 0; i < pool->primary_cache.size(); i++)
                    {
                        // Update used time
                        pool->primary_cache[i].last_used = GetCurrentTimestamp();
                    }
                }
                else
                {
                    // Not enough object
                    TimestampedObject obj(desc.create(), desc.destroy, GetCurrentTimestamp());
                    pool->primary_cache.push_back(obj);
                }
            }
            // LIFO
            TimestampedObject rc = pool->primary_cache.back();
            pool->primary_cache.pop_back();
            return rc.ptr.Leak();
        }

        void Put(const TypeDescriptor &desc, PoolDescriptor *pool, void *ptr)
        {
            TimestampedObject obj(ptr, desc.destroy, GetCurrentTimestamp());
            pool->primary_cache.push_back(obj);
            // Lazy wash out
            WashOutCache(pool);
        }

    }  // namespace tls
}  // namespace object_pool

#endif