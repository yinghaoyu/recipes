#include <assert.h>
#include <iostream>

class ErasedPtr
{
public:
    typedef void (*Deleter)(void *);

    ErasedPtr() : ptr_(NULL), deleter_(NULL) {}

    template <class T>
    explicit ErasedPtr(T *ptr) : ptr_(ptr), deleter_(&deleteT<T>)
    {
    }

    template <class T, class D>
    ErasedPtr(T *ptr, D deleter) : ptr_(ptr), deleter_(deleter)
    {
    }

    ~ErasedPtr()
    {
        if (ptr_)
        {
            deleter_(ptr_);
        }
    }

    void *Get() const { return ptr_; }

    template <class T>
    T *UncheckedGet() const
    {
        return reinterpret_cast<T *>(Get());
    }

    void Reset()
    {
        if (ptr_)
        {
            deleter_(ptr_);
            ptr_ = NULL;
        }
    }

    ErasedPtr(const ErasedPtr &other) : ptr_(other.ptr_), deleter_(other.deleter_) { const_cast<ErasedPtr &>(other).ptr_ = NULL; }

    ErasedPtr &operator=(const ErasedPtr &other)
    {
        if (this != &other)
        {
            Reset();
        }
        ptr_ = other.ptr_;
        deleter_ = other.deleter_;
        const_cast<ErasedPtr &>(other).ptr_ = NULL;
        return *this;
    }

    void *Leak()
    {
        void *leak = ptr_;
        ptr_ = NULL;
        return leak;
    }

    const Deleter GetDeleter() const { return deleter_; }

private:
    // Not copyable
    // ErasedPtr(const ErasedPtr&) {}
    // ErasedPtr& operator=(const ErasedPtr&) {}

    template <class T>
    static void deleteT(void *ptr)
    {
        delete static_cast<T *>(ptr);
    }

    void *ptr_;
    Deleter deleter_;
};