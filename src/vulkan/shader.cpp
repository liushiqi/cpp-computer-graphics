#include <application.hpp>
#include <logger.hpp>
#include <shader.hpp>

VkShaderStageFlagBits shader_type_to_stage(liu::shader_type type) {
  switch (type) {
  case liu::shader_type::VERTEX:
    return VK_SHADER_STAGE_VERTEX_BIT;
  case liu::shader_type::TESSELLATION_CONTROL:
    return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
  case liu::shader_type::TESSELLATION_EVALUATION:
    return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
  case liu::shader_type::GEOMETRY:
    return VK_SHADER_STAGE_GEOMETRY_BIT;
  case liu::shader_type::FRAGMENT:
    return VK_SHADER_STAGE_FRAGMENT_BIT;
  case liu::shader_type::COMPUTE:
    return VK_SHADER_STAGE_COMPUTE_BIT;
  case liu::shader_type::VK_RAY_RAYGEN:
    return VK_SHADER_STAGE_RAYGEN_BIT_KHR;
  case liu::shader_type::VK_RAY_ANY_HIT:
    return VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
  case liu::shader_type::VK_RAY_CLOSEST_HIT:
    return VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
  case liu::shader_type::VK_RAY_MISS:
    return VK_SHADER_STAGE_MISS_BIT_KHR;
  case liu::shader_type::VK_RAY_INTERSECTION:
    return VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
  case liu::shader_type::VK_RAY_CALLABLE:
    return VK_SHADER_STAGE_CALLABLE_BIT_KHR;
  }
  return VK_SHADER_STAGE_ALL;
}

VkFormat get_vk_format(const liu::input_format &format) {
  switch (format) {
  case liu::input_format::UINT_X32:
    return VK_FORMAT_R32_UINT;
  case liu::input_format::SINT_X32:
    return VK_FORMAT_R32_SINT;
  case liu::input_format::FLOAT_X32:
    return VK_FORMAT_R32_SFLOAT;
  case liu::input_format::UINT_X32Y32:
    return VK_FORMAT_R32G32_UINT;
  case liu::input_format::SINT_X32Y32:
    return VK_FORMAT_R32G32_SINT;
  case liu::input_format::FLOAT_X32Y32:
    return VK_FORMAT_R32G32_SFLOAT;
  case liu::input_format::UINT_X32Y32Z32:
    return VK_FORMAT_R32G32B32_UINT;
  case liu::input_format::SINT_X32Y32Z32:
    return VK_FORMAT_R32G32B32_SINT;
  case liu::input_format::FLOAT_X32Y32Z32:
    return VK_FORMAT_R32G32B32_SFLOAT;
  case liu::input_format::UINT_X32Y32Z32W32:
    return VK_FORMAT_R32G32B32A32_UINT;
  case liu::input_format::SINT_X32Y32Z32W32:
    return VK_FORMAT_R32G32B32A32_SINT;
  case liu::input_format::FLOAT_X32Y32Z32W32:
    return VK_FORMAT_R32G32B32A32_SFLOAT;
  case liu::input_format::UINT_X64:
    return VK_FORMAT_R64_UINT;
  case liu::input_format::SINT_X64:
    return VK_FORMAT_R64_SINT;
  case liu::input_format::FLOAT_X64:
    return VK_FORMAT_R64_SFLOAT;
  case liu::input_format::UINT_X64Y64:
    return VK_FORMAT_R64G64_UINT;
  case liu::input_format::SINT_X64Y64:
    return VK_FORMAT_R64G64_SINT;
  case liu::input_format::FLOAT_X64Y64:
    return VK_FORMAT_R64G64_SFLOAT;
  case liu::input_format::UINT_X64Y64Z64:
    return VK_FORMAT_R64G64B64_UINT;
  case liu::input_format::SINT_X64Y64Z64:
    return VK_FORMAT_R64G64B64_SINT;
  case liu::input_format::FLOAT_X64Y64Z64:
    return VK_FORMAT_R64G64B64_SFLOAT;
  case liu::input_format::UINT_X64Y64Z64W64:
    return VK_FORMAT_R64G64B64A64_UINT;
  case liu::input_format::SINT_X64Y64Z64W64:
    return VK_FORMAT_R64G64B64A64_SINT;
  case liu::input_format::FLOAT_X64Y64Z64W64:
    return VK_FORMAT_R64G64B64A64_SFLOAT;
  }
  return VK_FORMAT_UNDEFINED;
}

VkDescriptorType get_vk_uniform_type(const liu::uniform_type &type) {
  switch (type) {
  case liu::uniform_type::SAMPLER:
    return VK_DESCRIPTOR_TYPE_SAMPLER;
  case liu::uniform_type::COMBINED_IMAGE_SAMPLER:
    return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  case liu::uniform_type::SAMPLED_IMAGE:
    return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
  case liu::uniform_type::STORAGE_IMAGE:
    return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
  case liu::uniform_type::UNIFORM_TEXEL_BUFFER:
    return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
  case liu::uniform_type::STORAGE_TEXEL_BUFFER:
    return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
  case liu::uniform_type::UNIFORM_BUFFER:
    return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  case liu::uniform_type::STORAGE_BUFFER:
    return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  case liu::uniform_type::UNIFORM_BUFFER_DYNAMIC:
    return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
  case liu::uniform_type::STORAGE_BUFFER_DYNAMIC:
    return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
  case liu::uniform_type::INPUT_ATTACHMENT:
    return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
  case liu::uniform_type::ACCELERATION_STRUCTURE_KHR:
    return VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
  }
  return VK_DESCRIPTOR_TYPE_SAMPLER;
}

liu::shader::shader(const liu::base_application_t &app, const std::string &name, const liu::shader_input &input,
                    const liu::shader_uniform &uniform)
    : app(app), name(name) {
  VkResult result;
  std::vector<VkShaderModule> shader_modules;
  std::vector<VkPipelineShaderStageCreateInfo> shader_stages_info;

  for (auto type : liu::all_shader_types) {
    auto file = load_shader(app.get_assets_base_path(), name, type);
    if (file != std::nullopt) {
      auto real_file = file.value();
      VkShaderModuleCreateInfo create_info{.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                                           .pNext = nullptr,
                                           .flags = 0,
                                           .codeSize = real_file.size(),
                                           .pCode = (unsigned int *)real_file.data()};
      VkShaderModule shader_module;
      result = vkCreateShaderModule(app.get_vulkan_context().device, &create_info, nullptr, &shader_module);
      assert_log(result == VK_SUCCESS, "Failed to create shader module of shader {} in stage {} with error {}", name,
                 type, liu::vk_error_to_string(result));

      shader_modules.emplace_back(shader_module);
      VkPipelineShaderStageCreateInfo shader_stage_info{.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                                                        .pNext = nullptr,
                                                        .flags = 0,
                                                        .stage = shader_type_to_stage(type),
                                                        .module = shader_module,
                                                        .pName = "main",
                                                        .pSpecializationInfo = nullptr};
      shader_stages_info.emplace_back(shader_stage_info);
    }
  }

  VkVertexInputBindingDescription input_binding_info{
      .binding = 0, .stride = static_cast<uint32_t>(input.stride), .inputRate = VK_VERTEX_INPUT_RATE_VERTEX};
  std::vector<VkVertexInputAttributeDescription> input_attribute_info;
  for (auto &binding : input.bindings) {
    input_attribute_info.push_back({.location = binding.location,
                                    .binding = 0,
                                    .format = get_vk_format(binding.format),
                                    .offset = static_cast<uint32_t>(binding.offset)});
  }
  VkPipelineVertexInputStateCreateInfo vertex_input_info{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .vertexBindingDescriptionCount = 1,
      .pVertexBindingDescriptions = &input_binding_info,
      .vertexAttributeDescriptionCount = static_cast<uint32_t>(input_attribute_info.size()),
      .pVertexAttributeDescriptions = input_attribute_info.data()};

  VkPipelineInputAssemblyStateCreateInfo input_assembly_info{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
      .primitiveRestartEnable = VK_FALSE};

  VkViewport viewport{.x = 0.0f,
                      .y = 0.0f,
                      .width = static_cast<float>(app.get_vulkan_context().swap_chain_extent.width),
                      .height = static_cast<float>(app.get_vulkan_context().swap_chain_extent.height),
                      .minDepth = 0.0f,
                      .maxDepth = 1.0f};

  VkRect2D scissor{.offset = {0, 0}, .extent = app.get_vulkan_context().swap_chain_extent};

  VkPipelineViewportStateCreateInfo viewport_info{.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
                                                  .pNext = nullptr,
                                                  .flags = 0,
                                                  .viewportCount = 1,
                                                  .pViewports = &viewport,
                                                  .scissorCount = 1,
                                                  .pScissors = &scissor};

  VkPipelineRasterizationStateCreateInfo rasterization_info{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .depthClampEnable = VK_FALSE,
      .rasterizerDiscardEnable = VK_FALSE,
      .polygonMode = VK_POLYGON_MODE_FILL,
      .cullMode = VK_CULL_MODE_BACK_BIT,
      .frontFace = VK_FRONT_FACE_CLOCKWISE,
      .depthBiasEnable = VK_FALSE,
      .depthBiasConstantFactor = 0.0f,
      .depthBiasClamp = 0.0f,
      .depthBiasSlopeFactor = 0.0f,
      .lineWidth = 1.0f};

  VkPipelineMultisampleStateCreateInfo multisample_info{.sType =
                                                            VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
                                                        .pNext = nullptr,
                                                        .flags = 0,
                                                        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
                                                        .sampleShadingEnable = VK_FALSE,
                                                        .minSampleShading = 1.0f,
                                                        .pSampleMask = nullptr,
                                                        .alphaToCoverageEnable = VK_FALSE,
                                                        .alphaToOneEnable = VK_FALSE};

  VkPipelineColorBlendAttachmentState color_blend_state{.blendEnable = VK_TRUE,
                                                        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
                                                        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
                                                        .colorBlendOp = VK_BLEND_OP_ADD,
                                                        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
                                                        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
                                                        .alphaBlendOp = VK_BLEND_OP_ADD,
                                                        .colorWriteMask =
                                                            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                                            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT};

  VkPipelineColorBlendStateCreateInfo color_blend_info{.sType =
                                                           VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
                                                       .pNext = nullptr,
                                                       .flags = 0,
                                                       .logicOpEnable = VK_FALSE,
                                                       .logicOp = VK_LOGIC_OP_COPY,
                                                       .attachmentCount = 1,
                                                       .pAttachments = &color_blend_state,
                                                       .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}};

  VkDynamicState dynamic_states[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_LINE_WIDTH};

  VkPipelineDynamicStateCreateInfo dynamic_state_info{.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
                                                      .pNext = nullptr,
                                                      .flags = 0,
                                                      .dynamicStateCount = 2,
                                                      .pDynamicStates = dynamic_states};

  std::vector<VkDescriptorSetLayoutBinding> uniform_binding_info;
  for (auto &binding : uniform.bindings) {
    VkShaderStageFlags stage_flags = 0;
    for (auto &stage : binding.shaders) {
      stage_flags |= shader_type_to_stage(stage);
    }
    uniform_binding_info.push_back({.binding = binding.binding,
                                    .descriptorType = get_vk_uniform_type(binding.type),
                                    .descriptorCount = static_cast<uint32_t>(binding.count),
                                    .stageFlags = stage_flags,
                                    .pImmutableSamplers = nullptr});
  }
  VkDescriptorSetLayoutCreateInfo descriptor_layout_info{.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                                                         .pNext = nullptr,
                                                         .flags = 0,
                                                         .bindingCount = static_cast<uint32_t>(uniform.count),
                                                         .pBindings = uniform_binding_info.data()};
  result = vkCreateDescriptorSetLayout(app.get_vulkan_context().device, &descriptor_layout_info, nullptr,
                                       &descriptor_set_layout);
  assert_log(result == VK_SUCCESS, "Failed to create descriptor set layout with error {}", vk_error_to_string(result));
  VkPipelineLayoutCreateInfo pipeline_layout_info{.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
                                                  .pNext = nullptr,
                                                  .flags = 0,
                                                  .setLayoutCount = 1,
                                                  .pSetLayouts = &descriptor_set_layout,
                                                  .pushConstantRangeCount = 0,
                                                  .pPushConstantRanges = nullptr};
  result = vkCreatePipelineLayout(app.get_vulkan_context().device, &pipeline_layout_info, nullptr, &pipeline_layout);
  assert_log(result == VK_SUCCESS, "Failed to create pipeline layout with error {}", liu::vk_error_to_string(result));

  VkGraphicsPipelineCreateInfo pipeline_info{.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
                                             .pNext = nullptr,
                                             .flags = 0,
                                             .stageCount = static_cast<std::uint32_t>(shader_stages_info.size()),
                                             .pStages = shader_stages_info.data(),
                                             .pVertexInputState = &vertex_input_info,
                                             .pInputAssemblyState = &input_assembly_info,
                                             .pTessellationState = nullptr,
                                             .pViewportState = &viewport_info,
                                             .pRasterizationState = &rasterization_info,
                                             .pMultisampleState = &multisample_info,
                                             .pDepthStencilState = nullptr,
                                             .pColorBlendState = &color_blend_info,
                                             .pDynamicState = &dynamic_state_info,
                                             .layout = pipeline_layout,
                                             .renderPass = app.get_vulkan_context().render_pass,
                                             .subpass = 0,
                                             .basePipelineHandle = VK_NULL_HANDLE,
                                             .basePipelineIndex = -1};

  result = vkCreateGraphicsPipelines(app.get_vulkan_context().device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr,
                                     &graphics_pipeline);
  assert_log(result == VK_SUCCESS, "Failed to create graphics pipeline with error {}", liu::vk_error_to_string(result));

  for (auto module : shader_modules) {
    vkDestroyShaderModule(app.get_vulkan_context().device, module, nullptr);
  }
}

liu::shader::~shader() {
  vkDestroyPipeline(app.get_vulkan_context().device, graphics_pipeline, nullptr);
  vkDestroyPipelineLayout(app.get_vulkan_context().device, pipeline_layout, nullptr);
  vkDestroyDescriptorSetLayout(app.get_vulkan_context().device, descriptor_set_layout, nullptr);
}

void liu::shader::apply(std::function<void()> &callback) const {
  active();
  callback();
  inactive();
}

void liu::shader::active() const {
  for (size_t i = 0; i < app.get_vulkan_context().command_buffers.size(); i++) {
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(app.get_vulkan_context().command_buffers[i], &begin_info) != VK_SUCCESS) {
      throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = app.get_vulkan_context().render_pass;
    render_pass_info.framebuffer = app.get_vulkan_context().swap_chain_frame_buffers[i];
    render_pass_info.renderArea.offset = {0, 0};
    render_pass_info.renderArea.extent = app.get_vulkan_context().swap_chain_extent;

    VkClearValue clear_color = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    render_pass_info.clearValueCount = 1;
    render_pass_info.pClearValues = &clear_color;

    vkCmdBeginRenderPass(app.get_vulkan_context().command_buffers[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(app.get_vulkan_context().command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline);

    vkCmdDraw(app.get_vulkan_context().command_buffers[i], 3, 1, 0, 0);
  }
}

void liu::shader::inactive() const {
  for (auto command_buffer : app.get_vulkan_context().command_buffers) {
    vkCmdEndRenderPass(command_buffer);

    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer!");
    }
  }
}
