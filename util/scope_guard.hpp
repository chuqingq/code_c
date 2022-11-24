#pragma once

#include <cstdlib>    // std::size_t
#include <functional> // std::function

class ScopeGuard
{
public:
    ScopeGuard(std::function<void()> &&ptr)
        : ptr_(move(ptr))
    {
    }

    void dismiss() noexcept
    {
        dismissed_ = true;
    }

    ~ScopeGuard()
    {
        if (!dismissed_)
        {
            ptr_();
        }
    }

private:
    ScopeGuard(const ScopeGuard &) = delete;
    void *operator new(std::size_t) = delete;
    void operator delete(void *) = delete;

    std::function<void()> ptr_;
    bool dismissed_;
};
