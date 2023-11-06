#pragma once

#include "scope_guard.hpp"
// template <typename Fn>
// class Defer {
//   const Fn &fn_;

//  public:
//   Defer(const Fn &fn) noexcept : fn_(fn) {}

//   ~Defer() noexcept { fn_(); }

//   Defer(Defer &&) = delete;
// };

#define DEFER_PASTE_(x, y) x##y
#define DEFER_CAT_(x, y) DEFER_PASTE_(x, y)
#define DEFER(...)                                                             \
  ScopeGuard DEFER_CAT_(defer_, __LINE__) { __VA_ARGS__ }
