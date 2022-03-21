#pragma once

#include <memory>

namespace liu {
template<typename T>
class singleton {
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

  static void destroy() {
    inst.reset();
  }

  static T &get() { return *inst.get(); };
  singleton(const singleton &) = delete;
  singleton &operator=(const singleton &) = delete;

protected:
  singleton() = default;
  ~singleton() = default;

private:
  inline static std::once_flag init_flag;
  inline static std::unique_ptr<T> inst;
};
} // namespace liu
