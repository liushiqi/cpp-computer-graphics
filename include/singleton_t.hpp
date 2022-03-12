#pragma once

#include <memory>

namespace liu {
template<typename T>
class singleton_t {
public:
  template<typename... Args>
  requires std::is_constructible_v<T, Args...>
  static T &init(Args... args) { std::call_once(init_flag, inst, args...); }

  static T &get() { return inst; };
  singleton_t(const singleton_t &) = delete;
  singleton_t &operator=(const singleton_t &) = delete;

protected:
  singleton_t() = default;
  ~singleton_t() = default;

private:
  inline static std::once_flag init_flag;
  inline static std::shared_ptr<T> inst;
};
} // namespace liu
