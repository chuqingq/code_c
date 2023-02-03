#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include "uv.h"

namespace vs {
namespace rte {

class Component;

typedef struct {
  int timer_id;
  uv_timer_t timer_req;
  void* priv;
  std::function<void(int timer_id, void* priv)> proc;
} ComponentTimer;

inline void OnTimeOut(uv_timer_t* timer) {
  auto t = (ComponentTimer*)timer->data;
  t->proc(t->timer_id, t->priv);
}

class Component {
 public:
  inline Component(const std::string& name) : name_(name) {}
  inline virtual ~Component() {}

  virtual int OnCreate() = 0;

  // TODO 非线程安全
  int AddTimer(uint64_t period_us, int one_shot,
               std::function<void(int timer_id, void* priv)> proc, void* priv,
               int* timer_id) {
    timers_.push_back(ComponentTimer{});
    auto timer = &timers_[timers_.size() - 1];
    timer->timer_id = timers_.size() - 1;
    auto loop = uv_default_loop();
    uv_timer_init(loop, &timer->timer_req);
    timer->priv = priv;
    timer->proc = proc;
    timer->timer_req.data = timer;

    uv_timer_start(&timer->timer_req, OnTimeOut, period_us / 1000,
                   period_us / 1000);

    *timer_id = timer->timer_id;
    return 0;
  }

  int DelTimer(int timer_id) {
    auto t = &timers_[timer_id];
    return uv_timer_stop(&t->timer_req);
  }

  std::string name_;
  std::vector<ComponentTimer> timers_;
};

}  // namespace rte
}  // namespace vs
