#include <application.hpp>
#include <logger.hpp>

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

  if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
    error("Vulkan {}message: {}", type_str, data->pMessage);
  } else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    warn("Vulkan {}message: {}", type_str, data->pMessage);
  } else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
    info("Vulkan {}message: {}", type_str, data->pMessage);
  } else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
    debug("Vulkan {}message: {}", type_str, data->pMessage);
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

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

static VkPhysicalDevice select_device(VkInstance instance) {
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
    VkPhysicalDeviceProperties device_props;
    VkPhysicalDeviceFeatures device_features;
    VkPhysicalDeviceMemoryProperties memory_props;
    vkGetPhysicalDeviceProperties(device, &device_props);
    vkGetPhysicalDeviceFeatures(device, &device_features);
    vkGetPhysicalDeviceMemoryProperties(device, &memory_props);

    uint64_t heap_size = 0;
    auto heaps_pointer = memory_props.memoryHeaps;
    auto heaps = std::vector<VkMemoryHeap>(heaps_pointer, heaps_pointer + memory_props.memoryHeapCount);

    for (const auto &heap : heaps) {
      if (heap.flags & VkMemoryHeapFlagBits::VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
        heap_size += heap.size;
      }
    }

    info("Found GPU {} with heap size {}MiB", device_props.deviceName, heap_size / 1024 / 1024);

    if (device_props.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      if (device_features.geometryShader && heap_size > discrete_memory_size)
        discrete = device;
    } else if (device_props.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
      if (device_features.geometryShader && heap_size > integrated_memory_size)
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

static const char *layers_to_check[] = {"VK_LAYER_KHRONOS_validation", "VK_LAYER_LUNARG_standard_validation"};

static std::vector<const char *> get_available_validation_layers() {
  std::vector<const char *> enabled_layers;

  uint32_t available_layers_count;
  vkEnumerateInstanceLayerProperties(&available_layers_count, nullptr);

  std::vector<VkLayerProperties> available_layers(available_layers_count);
  vkEnumerateInstanceLayerProperties(&available_layers_count, available_layers.data());

  for (const auto &layer : layers_to_check) {
    for (const auto &available : available_layers) {
      if (strcmp(layer, available.layerName) == 0) {
        enabled_layers.push_back(layer);
      }
    }
  }
  return enabled_layers;
}

void liu::application::init_context() {
  if (!gladLoaderLoadVulkan(nullptr, nullptr, nullptr)) {
    fatal("GLAD load vulkan failed.");
    throw std::runtime_error("GLAD load vulkan failed.");
  }

  uint32_t extension_count = 0;
  const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&extension_count);

  std::vector<const char *> extensions(glfw_extensions, glfw_extensions + extension_count);

  VkApplicationInfo app_info{.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                             .pNext = nullptr,
                             .pApplicationName = title.data(),
                             .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
                             .pEngineName = "No Engine",
                             .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                             .apiVersion = VK_API_VERSION_1_2};

  VkInstanceCreateInfo instance_info{.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                                     .pNext = nullptr,
                                     .flags = 0,
                                     .pApplicationInfo = &app_info,
                                     .enabledLayerCount = 0,
                                     .ppEnabledLayerNames = nullptr,
                                     .enabledExtensionCount = extension_count,
                                     .ppEnabledExtensionNames = extensions.data()};

  VkDebugUtilsMessengerCreateInfoEXT debug_info;
  std::vector<const char *> layers;

  if (GLAD_VK_EXT_debug_utils && enable_validation_layers) {
    extension_count += 1;
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    layers = get_available_validation_layers();

    instance_info.enabledLayerCount = layers.size();
    instance_info.ppEnabledLayerNames = layers.data();
    instance_info.pNext = &debug_info;

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

  instance_info.enabledExtensionCount = extension_count;
  instance_info.ppEnabledExtensionNames = extensions.data();

  if (int result = vkCreateInstance(&instance_info, nullptr, &instance); result != VK_SUCCESS) {
    error("Vulkan failed to create a instance with error code {}", result);
    throw std::runtime_error("Vulkan failed to create a instance.");
  }

  physical_device = select_device(instance);

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
}

void liu::application::cleanup_context() {
  if (GLAD_VK_EXT_debug_utils && enable_validation_layers) {
    vkDestroyDebugUtilsMessengerEXT(instance, debug_messenger, nullptr);
  }
  vkDestroyInstance(instance, nullptr);

  info("Clean context finished.");
}
