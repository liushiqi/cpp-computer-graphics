#include <base_application.hpp>
#include <logger.hpp>
#include <set>

struct raw_str_less {
  inline bool operator()(const char *a, const char *b) const { return strcmp(a, b) < 0; }
};

static const char *const device_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME};

static const char *const layers_to_check[] = {"VK_LAYER_KHRONOS_validation", "VK_LAYER_LUNARG_standard_validation"};

static const char *const instance_extensions[] = {VK_EXT_DEBUG_UTILS_EXTENSION_NAME};

std::string liu::vk_error_to_string(VkResult error) {
  switch (error) {
#define STR(r)                                                                                                                                       \
  case VK_##r:                                                                                                                                       \
    return #r
    STR(NOT_READY);
    STR(TIMEOUT);
    STR(EVENT_SET);
    STR(EVENT_RESET);
    STR(INCOMPLETE);
    STR(ERROR_OUT_OF_HOST_MEMORY);
    STR(ERROR_OUT_OF_DEVICE_MEMORY);
    STR(ERROR_INITIALIZATION_FAILED);
    STR(ERROR_DEVICE_LOST);
    STR(ERROR_MEMORY_MAP_FAILED);
    STR(ERROR_LAYER_NOT_PRESENT);
    STR(ERROR_EXTENSION_NOT_PRESENT);
    STR(ERROR_FEATURE_NOT_PRESENT);
    STR(ERROR_INCOMPATIBLE_DRIVER);
    STR(ERROR_TOO_MANY_OBJECTS);
    STR(ERROR_FORMAT_NOT_SUPPORTED);
    STR(ERROR_FRAGMENTED_POOL);
    STR(ERROR_UNKNOWN);
    STR(ERROR_OUT_OF_POOL_MEMORY);
    STR(ERROR_INVALID_EXTERNAL_HANDLE);
    STR(ERROR_FRAGMENTATION);
    STR(ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS);
    STR(ERROR_SURFACE_LOST_KHR);
    STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
    STR(SUBOPTIMAL_KHR);
    STR(ERROR_OUT_OF_DATE_KHR);
#undef STR
  default:
    return "UNKNOWN_ERROR";
  }
}

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
    liu::clean_logger();
    throw std::runtime_error("GLFW initialization failed.");
  }

  if (!glfwVulkanSupported()) {
    fatal("Vulkan not supported.");
    liu::clean_logger();
    throw std::runtime_error("Vulkan not supported.");
  }

  int glfw_version_major, glfw_version_minor, glfw_version_rev;
  glfwGetVersion(&glfw_version_major, &glfw_version_minor, &glfw_version_rev);
  info("GLFW initialization succeeded, version: {}.{}.{}", glfw_version_major, glfw_version_minor, glfw_version_rev);

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

void liu::clean_context() { glfwTerminate(); }

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

static std::pair<std::vector<const char *>, std::optional<VkDebugUtilsMessengerCreateInfoEXT>> create_debug_util(bool enable_debugging) {
  std::vector<const char *> layers = get_available_validation_layers<std::size(layers_to_check)>(layers_to_check);

  if (enable_debugging) {
    return {layers, VkDebugUtilsMessengerCreateInfoEXT{
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
                    }};
  } else {
    return {layers, std::nullopt};
  }
}

static VkSurfaceKHR create_surface(GLFWwindow *window, VkInstance instance) {
  VkSurfaceKHR surface;
  if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
    fatal("Failed to create window surface.");
    liu::clean_logger();
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
    liu::clean_logger();
    throw std::runtime_error("No valid physical device for vulkan.");
  }

  std::vector<VkPhysicalDevice> devices(device_count);
  vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

  VkPhysicalDevice discrete = nullptr, integrated = nullptr;
  uint64_t discrete_memory_size = 0, integrated_memory_size = 0;

  for (const auto &device : devices) {
    VkPhysicalDeviceProperties device_props;
    vkGetPhysicalDeviceProperties(device, &device_props);

    const auto [graphics_family, present_family] = find_queue_indices(device, surface);
    if (!graphics_family) {
      info("Found Invalid GPU {} without graphics support.", device_props.deviceName);
      continue;
    } else if (!present_family) {
      info("Found Invalid GPU {} without present support.", device_props.deviceName);
      continue;
    }

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

    VkPhysicalDeviceFeatures device_features;
    VkPhysicalDeviceMemoryProperties memory_props;
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

    info("Found GPU {} with heap size {}MiB.", device_props.deviceName, heap_size / 1024 / 1024);

    if (device_props.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      if (heap_size > discrete_memory_size)
        discrete = device;
    } else if (device_props.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
      if (heap_size > integrated_memory_size)
        integrated = device;
    } else {
      warn("Unknown device: {}.", device_props.deviceName);
    }
  }

  if (discrete != nullptr) {
    VkPhysicalDeviceProperties device_props;
    vkGetPhysicalDeviceProperties(discrete, &device_props);
    info("Select discrete GPU: {}.", device_props.deviceName);
    return discrete;
  } else if (integrated != nullptr) {
    VkPhysicalDeviceProperties device_props;
    vkGetPhysicalDeviceProperties(integrated, &device_props);
    info("Select discrete GPU: {}.", device_props.deviceName);
    return integrated;
  } else {
    fatal("No valid vulkan device found.");
    liu::clean_logger();
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
  device_features.geometryShader = VK_TRUE;

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

  if (VkResult result = vkCreateDevice(physical_device, &device_info, nullptr, &device); result != VK_SUCCESS) {
    fatal("Failed to create logical device with error {}.", liu::vk_error_to_string(result));
    liu::clean_logger();
    throw std::runtime_error("Failed to create logical device.");
  }

  return device;
}

std::tuple<VkSwapchainKHR, std::vector<VkImage>, VkFormat, VkExtent2D>
create_swap_chain(GLFWwindow *window, VkPhysicalDevice physical_device, VkDevice device, VkSurfaceKHR surface, const uint32_t queue_family[2]) {
  VkSurfaceCapabilitiesKHR capabilities;

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &capabilities);

  uint32_t format_count;
  std::vector<VkSurfaceFormatKHR> formats;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, nullptr);

  if (format_count != 0) {
    formats.resize(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, formats.data());
  } else {
    error("No surface format found.");
    liu::clean_logger();
    throw std::runtime_error("No surface format found.");
  }
  VkSurfaceFormatKHR format = formats[0];

  for (const auto &format_iter : formats) {
    if (format_iter.format == VK_FORMAT_B8G8R8A8_SRGB && format_iter.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      format = format_iter;
      break;
    }
  }

  uint32_t present_mode_count;
  std::vector<VkPresentModeKHR> present_modes;
  VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
  vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, nullptr);

  if (present_mode_count != 0) {
    present_modes.resize(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, present_modes.data());
  }

  for (const auto &mode_iter : present_modes) {
    if (mode_iter == VK_PRESENT_MODE_MAILBOX_KHR) {
      present_mode = mode_iter;
    }
  }

  VkExtent2D extent;
  if (capabilities.currentExtent.width != UINT32_MAX) {
    extent = capabilities.currentExtent;
  } else {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    VkExtent2D actual_extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

    actual_extent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actual_extent.width));
    actual_extent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actual_extent.height));

    extent = actual_extent;
  }

  uint32_t image_count = capabilities.maxImageCount > 0 && capabilities.minImageCount + 1 > capabilities.maxImageCount
                             ? capabilities.maxImageCount
                             : capabilities.minImageCount + 1;
  bool graphics_present_family_same = queue_family[0] == queue_family[1];
  VkSwapchainCreateInfoKHR swap_chain_create_info{.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                                                  .pNext = nullptr,
                                                  .flags = 0,
                                                  .surface = surface,
                                                  .minImageCount = capabilities.minImageCount + 1,
                                                  .imageFormat = format.format,
                                                  .imageColorSpace = format.colorSpace,
                                                  .imageExtent = extent,
                                                  .imageArrayLayers = 1,
                                                  .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                                                  .imageSharingMode =
                                                      graphics_present_family_same ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
                                                  .queueFamilyIndexCount = uint32_t(graphics_present_family_same ? 0 : 2),
                                                  .pQueueFamilyIndices = graphics_present_family_same ? nullptr : queue_family,
                                                  .preTransform = capabilities.currentTransform,
                                                  .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                                                  .presentMode = present_mode,
                                                  .clipped = VK_TRUE,
                                                  .oldSwapchain = VK_NULL_HANDLE};

  VkSwapchainKHR swap_chain;

  if (VkResult result = vkCreateSwapchainKHR(device, &swap_chain_create_info, nullptr, &swap_chain); result != VK_SUCCESS) {
    error("Swap chain create failed with error {}", liu::vk_error_to_string(result));
    liu::clean_logger();
    throw std::runtime_error("Swap chain create failed.");
  }

  std::vector<VkImage> images;
  vkGetSwapchainImagesKHR(device, swap_chain, &image_count, nullptr);
  images.resize(image_count);
  vkGetSwapchainImagesKHR(device, swap_chain, &image_count, images.data());

  return {swap_chain, images, format.format, extent};
}

std::vector<VkImageView> create_image_view(VkDevice device, const std::vector<VkImage> &images, VkFormat format) {
  std::vector<VkImageView> image_views(images.size());
  for (size_t i = 0; i < image_views.size(); i++) {
    VkImageViewCreateInfo create_info{
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .image = images[i],
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = format,
        .components = {.r = VK_COMPONENT_SWIZZLE_IDENTITY,
                       .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                       .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                       .a = VK_COMPONENT_SWIZZLE_IDENTITY},
        .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1}};

    if (VkResult result = vkCreateImageView(device, &create_info, nullptr, &image_views[i]); result != VK_SUCCESS) {
      error("Failed to create image views with error {}", liu::vk_error_to_string(result));
      liu::clean_logger();
      throw std::runtime_error("Failed to create image views.");
    }
  }

  return image_views;
}

VkRenderPass create_render_pass(VkDevice device, VkFormat swap_chain_image_format) {
  VkRenderPass render_pass;
  VkAttachmentDescription color_attachment{.flags = 0,
                                           .format = swap_chain_image_format,
                                           .samples = VK_SAMPLE_COUNT_1_BIT,
                                           .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                                           .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                                           .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                                           .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                           .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                           .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR};

  VkAttachmentReference color_attachment_ref{.attachment = 0, .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};

  VkSubpassDescription subpass{.flags = 0,
                               .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
                               .inputAttachmentCount = 0,
                               .pInputAttachments = nullptr,
                               .colorAttachmentCount = 1,
                               .pColorAttachments = &color_attachment_ref,
                               .pResolveAttachments = nullptr,
                               .pDepthStencilAttachment = nullptr,
                               .preserveAttachmentCount = 0,
                               .pPreserveAttachments = nullptr};

  VkRenderPassCreateInfo render_pass_info{.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
                                          .pNext = nullptr,
                                          .flags = 0,
                                          .attachmentCount = 1,
                                          .pAttachments = &color_attachment,
                                          .subpassCount = 1,
                                          .pSubpasses = &subpass,
                                          .dependencyCount = 0,
                                          .pDependencies = nullptr};
  if (VkResult result = vkCreateRenderPass(device, &render_pass_info, nullptr, &render_pass); result != VK_SUCCESS) {
    error("Failed to create render pass with error {}", liu::vk_error_to_string(result));
    liu::clean_logger();
    throw std::runtime_error("Failed to create render pass.");
  }

  return render_pass;
}

std::vector<VkFramebuffer> create_framebuffer(VkDevice device, const std::vector<VkImageView> &image_view, VkExtent2D extent,
                                              VkRenderPass render_pass) {
  std::vector<VkFramebuffer> frame_buffers;
  for (auto &i : image_view) {
    VkImageView attachments[] = {i};
    VkFramebuffer framebuffer;

    VkFramebufferCreateInfo framebuffer_info{.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                                             .pNext = nullptr,
                                             .flags = 0,
                                             .renderPass = render_pass,
                                             .attachmentCount = 1,
                                             .pAttachments = attachments,
                                             .width = extent.width,
                                             .height = extent.height,
                                             .layers = 1};

    if (VkResult result = vkCreateFramebuffer(device, &framebuffer_info, nullptr, &framebuffer); result != VK_SUCCESS) {
      error("Failed to create framebuffer with error {}", liu::vk_error_to_string(result));
      liu::clean_logger();
      throw std::runtime_error("Failed to create framebuffer.");
    }

    frame_buffers.emplace_back(framebuffer);
  }

  return frame_buffers;
}

VkCommandPool create_command_pool(VkDevice device, const uint32_t queue_family[2]) {
  VkCommandPool command_pool;
  VkCommandPoolCreateInfo command_pool_info{
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO, .pNext = nullptr, .flags = 0, .queueFamilyIndex = queue_family[0]};
  if (VkResult result = vkCreateCommandPool(device, &command_pool_info, nullptr, &command_pool); result != VK_SUCCESS) {
    error("Failed to create command pool with error {}", liu::vk_error_to_string(result));
    liu::clean_logger();
    throw std::runtime_error("Failed to create command pool.");
  }
  return command_pool;
}

std::vector<VkCommandBuffer> create_command_buffers(VkDevice device, uint32_t size, VkCommandPool command_pool) {
  std::vector<VkCommandBuffer> command_buffer;
  command_buffer.resize(size);
  VkCommandBufferAllocateInfo command_buffer_info{.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                                                  .pNext = nullptr,
                                                  .commandPool = command_pool,
                                                  .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                                  .commandBufferCount = (uint32_t)command_buffer.size()};
  if (VkResult result = vkAllocateCommandBuffers(device, &command_buffer_info, command_buffer.data()); result != VK_SUCCESS) {
    error("Failed to create command pool with error {}", liu::vk_error_to_string(result));
    liu::clean_logger();
    throw std::runtime_error("Failed to create command pool.");
  }

  return command_buffer;
}

void liu::base_application::init_context() {
  if (!gladLoadVulkanUserPtr(nullptr, reinterpret_cast<GLADuserptrloadfunc>(glfwGetInstanceProcAddress), nullptr)) {
    fatal("GLAD load vulkan failed.");
    liu::clean_logger();
    throw std::runtime_error("GLAD load vulkan failed.");
  }

  uint32_t extension_count = 0;
  const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&extension_count);

  std::set<std::string> extensions_set(glfw_extensions, glfw_extensions + extension_count);

  auto [layers, debug_info] = create_debug_util(GLAD_VK_EXT_debug_utils && liu::base_application::enable_validation_layers);

  if (!debug_info.has_value()) {
    extension_count += 1;
    extensions_set.emplace(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  for (const auto extension : instance_extensions) {
    extensions_set.emplace(extension);
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
                             .pEngineName = "",
                             .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                             .apiVersion = VK_API_VERSION_1_2};

  VkInstanceCreateInfo instance_info{.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                                     .pNext = debug_info.has_value() ? &debug_info.value() : nullptr,
                                     .flags = 0,
                                     .pApplicationInfo = &app_info,
                                     .enabledLayerCount = static_cast<uint32_t>(layers.size()),
                                     .ppEnabledLayerNames = layers.empty() ? nullptr : layers.data(),
                                     .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
                                     .ppEnabledExtensionNames = extensions.data()};

  if (VkResult result = vkCreateInstance(&instance_info, nullptr, &instance); result != VK_SUCCESS) {
    error("Vulkan failed to create a instance with error {}", liu::vk_error_to_string(result));
    liu::clean_logger();
    throw std::runtime_error("Vulkan failed to create a instance.");
  }

  this->surface = create_surface(window, instance);

  this->physical_device = select_physical_device(instance, surface);

  if (!gladLoadVulkanUserPtr(physical_device, reinterpret_cast<GLADuserptrloadfunc>(glfwGetInstanceProcAddress), instance)) {
    fatal("GLAD load vulkan failed.");
    liu::clean_logger();
    throw std::runtime_error("GLAD load vulkan failed.");
  }

  info("GLAD load vulkan succeeded.");

  if (debug_info.has_value()) {
    if (VkResult result = vkCreateDebugUtilsMessengerEXT(instance, &debug_info.value(), nullptr, &debug_messenger); result != VK_SUCCESS) {
      fatal("Vulkan debug messenger failed to init with error {}.", liu::vk_error_to_string(result));
      liu::clean_logger();
      throw std::runtime_error("Vulkan debug messenger failed to init.");
    }
  }

  const auto [graphics_family, present_family] = find_queue_indices(physical_device, surface);
  this->graphics_family = graphics_family;
  this->present_family = present_family;

  this->device = create_logical_device(graphics_family, present_family, physical_device);

  vkGetDeviceQueue(device, graphics_family.value(), 0, &graphics_queue);
  vkGetDeviceQueue(device, present_family.value(), 0, &present_queue);

  uint32_t queue_family[2] = {graphics_family.value(), present_family.value()};
  auto [swap_chain, swap_chain_images, swap_chain_image_format, swap_chain_extent] =
      create_swap_chain(window, physical_device, device, surface, queue_family);
  this->swap_chain = swap_chain;
  this->swap_chain_images = swap_chain_images;
  this->swap_chain_image_format = swap_chain_image_format;
  this->swap_chain_extent = swap_chain_extent;

  this->swap_chain_image_view = create_image_view(device, swap_chain_images, swap_chain_image_format);

  this->render_pass = create_render_pass(device, swap_chain_image_format);

  this->swap_chain_frame_buffers = create_framebuffer(device, swap_chain_image_view, swap_chain_extent, render_pass);

  this->command_pool = create_command_pool(device, queue_family);

  this->command_buffers = create_command_buffers(device, static_cast<uint32_t>(swap_chain_frame_buffers.size()), command_pool);
}

void liu::base_application::clean_context() {
  vkDestroyCommandPool(device, command_pool, nullptr);
  for (auto framebuffer : swap_chain_frame_buffers) {
    vkDestroyFramebuffer(device, framebuffer, nullptr);
  }
  vkDestroyRenderPass(device, render_pass, nullptr);
  for (auto image_view : swap_chain_image_view) {
    vkDestroyImageView(device, image_view, nullptr);
  }
  vkDestroySwapchainKHR(device, swap_chain, nullptr);
  vkDestroyDevice(device, nullptr);
  if (GLAD_VK_EXT_debug_utils && enable_validation_layers) {
    vkDestroyDebugUtilsMessengerEXT(instance, debug_messenger, nullptr);
  }
  vkDestroySurfaceKHR(instance, surface, nullptr);
  vkDestroyInstance(instance, nullptr);

  info("Clean context finished.");
}
