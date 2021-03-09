#include <base_application.hpp>
#include <logger.hpp>
#include <set>

struct raw_str_less {
  inline bool operator()(const char *a, const char *b) const { return strcmp(a, b) < 0; }
};

static const char *const device_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

static const char *const layers_to_check[] = {"VK_LAYER_KHRONOS_validation", "VK_LAYER_LUNARG_standard_validation"};

static const char *const instance_extensions[] = {VK_EXT_DEBUG_UTILS_EXTENSION_NAME};

static VKAPI_ATTR VkBool32 VKAPI_CALL vulkan_debug_message_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                                                    VkDebugUtilsMessageTypeFlagsEXT type,
                                                                    const VkDebugUtilsMessengerCallbackDataEXT *data,
                                                                    [[maybe_unused]] void *user_data) {
  std::string type_str;
  if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
    type_str += "general ";
  }
  if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
    type_str += "validation ";
  }
  if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
    type_str += "performance ";
  }

  std::string message(data->pMessage);
  if (message.back() == '\n') {
    message.pop_back();
  }

  if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
    error("Vulkan {}message from {}: {}", type_str, data->pMessageIdName, message);
  } else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    warn("Vulkan {}message from {}: {}", type_str, data->pMessageIdName, message);
  } else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
    info("Vulkan {}message from {}: {}", type_str, data->pMessageIdName, message);
  } else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
    trace("Vulkan {}message from {}: {}", type_str, data->pMessageIdName, message);
  }
  return VK_FALSE;
}

void liu::init_context() {
  if (!glfwInit()) {
    fatal("GLFW initialization failed.");
    throw std::runtime_error("GLFW initialization failed.");
  }

  if (!glfwVulkanSupported()) {
    fatal("Vulkan not supported.");
    throw std::runtime_error("Vulkan not supported.");
  }

  int glfw_version_major, glfw_version_minor, glfw_version_rev;
  glfwGetVersion(&glfw_version_major, &glfw_version_minor, &glfw_version_rev);
  info("GLFW initialization succeeded, version: {}.{}.{}", glfw_version_major, glfw_version_minor, glfw_version_rev);

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

void liu::clean_context() {
  glfwTerminate();
  gladLoaderUnloadVulkan();
}

template<std::size_t i>
static std::vector<const char *> get_available_validation_layers(const char *const (&to_check)[i]) {
  std::set<const char *, raw_str_less> enabled_layers;

  uint32_t available_layers_count;
  vkEnumerateInstanceLayerProperties(&available_layers_count, nullptr);

  std::vector<VkLayerProperties> available_layers(available_layers_count);
  vkEnumerateInstanceLayerProperties(&available_layers_count, available_layers.data());

  for (const auto &layer : to_check) {
    for (const auto &available : available_layers) {
      if (strcmp(layer, available.layerName) == 0) {
        enabled_layers.emplace(layer);
      }
    }
  }
  return std::vector(enabled_layers.begin(), enabled_layers.end());
}

static VkSurfaceKHR create_surface(VkInstance instance, GLFWwindow *window) {
  VkSurfaceKHR surface;
  if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
    fatal("Failed to create window surface.");
    throw std::runtime_error("Failed to create window surface.");
  }
  return surface;
}

static std::tuple<std::optional<uint32_t>, std::optional<uint32_t>> find_queue_indices(VkPhysicalDevice device, VkSurfaceKHR surface) {
  std::optional<uint32_t> graphics_family, present_family;
  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

  std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

  for (uint32_t i = 0; i < queue_family_count; ++i) {
    if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      graphics_family = i;
    }

    VkBool32 present_support = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present_support);

    if (present_support) {
      present_family = i;
    }

    if (graphics_family && present_family) {
      break;
    }
  }

  trace("Selected graphics family No.{}, present family No.{}", graphics_family.has_value() ? std::to_string(graphics_family.value()) : "(null)",
        present_family.has_value() ? std::to_string(present_family.value()) : "(null)");

  return {graphics_family, present_family};
}

static VkPhysicalDevice select_physical_device(VkInstance instance, VkSurfaceKHR surface) {
  uint32_t device_count;
  vkEnumeratePhysicalDevices(instance, &device_count, nullptr);

  if (device_count == 0) {
    fatal("No valid physical device for vulkan.");
    throw std::runtime_error("No valid physical device for vulkan.");
  }

  std::vector<VkPhysicalDevice> devices(device_count);
  vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

  VkPhysicalDevice discrete = nullptr, integrated = nullptr;
  uint64_t discrete_memory_size = 0, integrated_memory_size = 0;

  for (const auto &device : devices) {
    const auto [graphics_family, present_family] = find_queue_indices(device, surface);
    if (!(graphics_family && present_family))
      continue;

    uint32_t extension_count;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);
    std::vector<VkExtensionProperties> available_extensions(extension_count);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());
    std::set<const char *, raw_str_less> required_extensions(std::begin(device_extensions), std::end(device_extensions));
    for (const auto &extension : available_extensions) {
      required_extensions.erase(extension.extensionName);
    }
    if (!required_extensions.empty()) {
      continue;
    }

    VkPhysicalDeviceProperties device_props;
    VkPhysicalDeviceFeatures device_features;
    VkPhysicalDeviceMemoryProperties memory_props;
    vkGetPhysicalDeviceProperties(device, &device_props);
    vkGetPhysicalDeviceFeatures(device, &device_features);
    vkGetPhysicalDeviceMemoryProperties(device, &memory_props);

    if (!device_features.geometryShader)
      continue;

    uint64_t heap_size = 0;
    auto heaps_pointer = memory_props.memoryHeaps;
    auto heaps = std::vector<VkMemoryHeap>(heaps_pointer, heaps_pointer + memory_props.memoryHeapCount);

    for (const auto &heap : heaps) {
      if (heap.flags & VkMemoryHeapFlagBits::VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
        heap_size += heap.size;
      }
    }

    trace("Found GPU {} with heap size {}MiB", device_props.deviceName, heap_size / 1024 / 1024);

    if (device_props.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      if (heap_size > discrete_memory_size)
        discrete = device;
    } else if (device_props.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
      if (heap_size > integrated_memory_size)
        integrated = device;
    } else {
      warn("Unknown device: {}", device_props.deviceName);
    }
  }

  if (discrete != nullptr) {
    VkPhysicalDeviceProperties device_props;
    vkGetPhysicalDeviceProperties(discrete, &device_props);
    info("Select discrete GPU: {}", device_props.deviceName);
    return discrete;
  } else if (integrated != nullptr) {
    VkPhysicalDeviceProperties device_props;
    vkGetPhysicalDeviceProperties(integrated, &device_props);
    info("Select discrete GPU: {}", device_props.deviceName);
    return integrated;
  } else {
    fatal("No valid vulkan device found.");
    throw std::runtime_error("No valid vulkan device found.");
  }
}

VkDevice create_logical_device(std::optional<uint32_t> graphics_family, std::optional<uint32_t> present_family, VkPhysicalDevice physical_device) {
  VkDevice device;

  std::vector<VkDeviceQueueCreateInfo> queue_infos;
  std::set<uint32_t> unique_indices = {graphics_family.value(), present_family.value()};

  float queue_priority = 1.0f;
  for (uint32_t queue_family : unique_indices) {
    VkDeviceQueueCreateInfo queue_info{.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                                       .pNext = nullptr,
                                       .flags = 0,
                                       .queueFamilyIndex = queue_family,
                                       .queueCount = 1,
                                       .pQueuePriorities = &queue_priority};
    queue_infos.push_back(queue_info);
  }

  VkPhysicalDeviceFeatures device_features{};

  std::vector<const char *> layers;
  if (liu::base_application::enable_validation_layers) {
    layers = get_available_validation_layers<std::size(layers_to_check)>(layers_to_check);
  }

  VkDeviceCreateInfo device_info{.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                                 .pNext = nullptr,
                                 .flags = 0,
                                 .queueCreateInfoCount = static_cast<uint32_t>(queue_infos.size()),
                                 .pQueueCreateInfos = queue_infos.data(),
                                 .enabledLayerCount = static_cast<uint32_t>(layers.size()),
                                 .ppEnabledLayerNames = layers.empty() ? nullptr : layers.data(),
                                 .enabledExtensionCount = std::size(device_extensions),
                                 .ppEnabledExtensionNames = device_extensions,
                                 .pEnabledFeatures = &device_features};

  if (vkCreateDevice(physical_device, &device_info, nullptr, &device) != VK_SUCCESS) {
    fatal("Failed to create logical device.");
    throw std::runtime_error("Failed to create logical device.");
  }

  return device;
}

std::tuple<VkSurfaceCapabilitiesKHR, std::vector<VkSurfaceFormatKHR>, std::vector<VkPresentModeKHR>> create_swap_chain(VkPhysicalDevice device,
                                                                                                                       VkSurfaceKHR surface) {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> present_modes;

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &capabilities);

  uint32_t format_count;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr);

  if (format_count != 0) {
    formats.resize(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, formats.data());
  }

  uint32_t present_mode_count;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, nullptr);

  if (present_mode_count != 0) {
    present_modes.resize(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, present_modes.data());
  }

  return {capabilities, formats, present_modes};
}

void liu::base_application::init_context() {
  if (!gladLoaderLoadVulkan(nullptr, nullptr, nullptr)) {
    fatal("GLAD load vulkan failed.");
    throw std::runtime_error("GLAD load vulkan failed.");
  }

  uint32_t extension_count = 0;
  const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&extension_count);

  std::set<std::string> extensions_set(glfw_extensions, glfw_extensions + extension_count);

  for (const auto extension : instance_extensions) {
    extensions_set.emplace(extension);
  }

  VkDebugUtilsMessengerCreateInfoEXT debug_info;
  std::vector<const char *> layers;

  if (GLAD_VK_EXT_debug_utils && enable_validation_layers) {
    extension_count += 1;
    extensions_set.emplace(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    layers = get_available_validation_layers<std::size(layers_to_check)>(layers_to_check);

    debug_info = {
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .pNext = nullptr,
      .flags = 0,
      .messageSeverity = 0u
#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_DEBUG
                         | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
#endif
#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_INFO
                         | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
#endif
#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_WARN
                         | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
#endif
#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_ERROR
                         | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
#endif
      ,
      .messageType = 0u | VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
      .pfnUserCallback = vulkan_debug_message_callback,
      .pUserData = nullptr,
    };
  }

  std::vector<const char *> extensions;
  extensions.reserve(extensions_set.size());
  for (const auto &extension : extensions_set) {
    extensions.emplace_back(extension.data());
  }

  VkApplicationInfo app_info{.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                             .pNext = nullptr,
                             .pApplicationName = title.data(),
                             .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
                             .pEngineName = "No Engine",
                             .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                             .apiVersion = VK_API_VERSION_1_2};

  VkInstanceCreateInfo instance_info{.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                                     .pNext = GLAD_VK_EXT_debug_utils && enable_validation_layers ? &debug_info : nullptr,
                                     .flags = 0,
                                     .pApplicationInfo = &app_info,
                                     .enabledLayerCount = static_cast<uint32_t>(layers.size()),
                                     .ppEnabledLayerNames = layers.empty() ? nullptr : layers.data(),
                                     .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
                                     .ppEnabledExtensionNames = extensions.data()};

  if (int result = vkCreateInstance(&instance_info, nullptr, &instance); result != VK_SUCCESS) {
    error("Vulkan failed to create a instance with error code {}", result);
    throw std::runtime_error("Vulkan failed to create a instance.");
  }

  surface = create_surface(instance, window);

  physical_device = select_physical_device(instance, surface);

  if (!gladLoaderLoadVulkan(instance, physical_device, nullptr)) {
    fatal("GLAD load vulkan failed.");
    throw std::runtime_error("GLAD load vulkan failed.");
  }

  info("GLAD load vulkan succeeded.");

  if (GLAD_VK_EXT_debug_utils && enable_validation_layers) {
    if (vkCreateDebugUtilsMessengerEXT(instance, &debug_info, nullptr, &debug_messenger) != VK_SUCCESS) {
      fatal("Vulkan debug messenger failed to init.");
      throw std::runtime_error("Vulkan debug messenger failed to init.");
    }
  }

  const auto [graphics_family, present_family] = find_queue_indices(physical_device, surface);
  this->graphics_family = graphics_family;
  this->present_family = present_family;

  device = create_logical_device(graphics_family, present_family, physical_device);

  vkGetDeviceQueue(device, graphics_family.value(), 0, &graphics_queue);
  vkGetDeviceQueue(device, present_family.value(), 0, &present_queue);
}

void liu::base_application::cleanup_context() {
  vkDestroyDevice(device, nullptr);
  if (GLAD_VK_EXT_debug_utils && enable_validation_layers) {
    vkDestroyDebugUtilsMessengerEXT(instance, debug_messenger, nullptr);
  }
  vkDestroySurfaceKHR(instance, surface, nullptr);
  vkDestroyInstance(instance, nullptr);

  info("Clean context finished.");
}
