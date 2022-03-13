#pragma once

#include <memory>

namespace liu {
template<typename T>
class singleton_t {
public:
  template<typename... Args>
  static T &init(Args... args) {
    std::call_once(
        init_flag, [](Args... args) { inst.reset(new T(std::forward<Args>(args)...)); }, std::forward<Args>(args)...);
    return *inst.get();
  }

  static T &init() {
    std::call_once(init_flag, []() { inst.reset(new T); });
    return *inst.get();
  }

  static T &get() { return *inst.get(); };
  singleton_t(const singleton_t &) = delete;
  singleton_t &operator=(const singleton_t &) = delete;

protected:
  singleton_t() = default;
  ~singleton_t() = default;

private:
  inline static std::once_flag init_flag;
  inline static std::unique_ptr<T> inst;
};
} // namespace liu
