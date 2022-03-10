#include <logger.hpp>
#include <shader.hpp>
#include <spirv_reflect.h>

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

std::pair<std::map<std::string, std::int32_t>, std::map<std::string, std::int32_t>>
build_indices(const std::vector<std::uint8_t> &byte_code) {
  spv_reflect::ShaderModule module(byte_code);
  assert_log(module.GetResult() == SPV_REFLECT_RESULT_SUCCESS, "SPIR-V reflection load failed.");
  return std::make_pair(std::map<std::string, std::int32_t>(), std::map<std::string, std::int32_t>());
}

liu::shader::shader(const liu::base_application &app, const std::string &name) : app(app), name(name) {
  VkResult result;
  std::vector<VkShaderModule> shader_modules;
  std::vector<VkPipelineShaderStageCreateInfo> shader_stages_info;

  for (auto type : liu::all_shader_types) {
    auto file = load_shader(app.assets_base_path, name, type);
    if (file != std::nullopt) {
      auto real_file = file.value();
      VkShaderModuleCreateInfo create_info{.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                                           .pNext = nullptr,
                                           .flags = 0,
                                           .codeSize = real_file.size(),
                                           .pCode = (unsigned int *)real_file.data()};
      VkShaderModule shader_module;
      result = vkCreateShaderModule(app.device, &create_info, nullptr, &shader_module);
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

      build_indices(real_file);
    }
  }

  VkPipelineVertexInputStateCreateInfo vertex_input_info{.sType =
                                                             VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                                                         .pNext = nullptr,
                                                         .flags = 0,
                                                         .vertexBindingDescriptionCount = 0,
                                                         .pVertexBindingDescriptions = nullptr,
                                                         .vertexAttributeDescriptionCount = 0,
                                                         .pVertexAttributeDescriptions = nullptr};

  VkPipelineInputAssemblyStateCreateInfo input_assemply_info{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
      .primitiveRestartEnable = VK_FALSE};

  VkViewport viewport{.x = 0.0f,
                      .y = 0.0f,
                      .width = static_cast<float>(app.swap_chain_extent.width),
                      .height = static_cast<float>(app.swap_chain_extent.height),
                      .minDepth = 0.0f,
                      .maxDepth = 1.0f};

  VkRect2D scissor{.offset = {0, 0}, .extent = app.swap_chain_extent};

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

  VkPipelineLayoutCreateInfo pipeline_layout_info{.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
                                                  .pNext = nullptr,
                                                  .flags = 0,
                                                  .setLayoutCount = 0,
                                                  .pSetLayouts = nullptr,
                                                  .pushConstantRangeCount = 0,
                                                  .pPushConstantRanges = nullptr};
  result = vkCreatePipelineLayout(app.device, &pipeline_layout_info, nullptr, &pipeline_layout);
  assert_log(result == VK_SUCCESS, "Failed to create pipeline layout with error {}", liu::vk_error_to_string(result));

  VkGraphicsPipelineCreateInfo pipeline_info{.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
                                             .pNext = nullptr,
                                             .flags = 0,
                                             .stageCount = static_cast<uint32_t>(shader_stages_info.size()),
                                             .pStages = shader_stages_info.data(),
                                             .pVertexInputState = &vertex_input_info,
                                             .pInputAssemblyState = &input_assemply_info,
                                             .pTessellationState = nullptr,
                                             .pViewportState = &viewport_info,
                                             .pRasterizationState = &rasterization_info,
                                             .pMultisampleState = &multisample_info,
                                             .pDepthStencilState = nullptr,
                                             .pColorBlendState = &color_blend_info,
                                             .pDynamicState = &dynamic_state_info,
                                             .layout = pipeline_layout,
                                             .renderPass = app.render_pass,
                                             .subpass = 0,
                                             .basePipelineHandle = VK_NULL_HANDLE,
                                             .basePipelineIndex = -1};

  result = vkCreateGraphicsPipelines(app.device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &graphics_pipeline);
  assert_log(result == VK_SUCCESS, "Failed to create graphics pipeline with error {}", liu::vk_error_to_string(result));

  for (auto module : shader_modules) {
    vkDestroyShaderModule(app.device, module, nullptr);
  }
}

liu::shader::~shader() {
  vkDestroyPipeline(app.device, graphics_pipeline, nullptr);
  vkDestroyPipelineLayout(app.device, pipeline_layout, nullptr);
}

void liu::shader::apply(std::function<void()> &callback) const {
  active();
  callback();
  inactive();
}

std::optional<std::int32_t> liu::shader::get_attribute_index(const std::string &attribute_name) const {
  try {
    return attribute_indices.at(attribute_name);
  } catch (const std::out_of_range &e) {
    warn("Requesting non-existing attribute {} in shader {}", attribute_name, name);
    return std::nullopt;
  }
}

std::optional<std::int32_t> liu::shader::get_uniform_index(const std::string &uniform_name) const {
  try {
    return uniform_indices.at(uniform_name);
  } catch (const std::out_of_range &e) {
    warn("Requesting non-existing uniform {} in shader {}", uniform_name, name);
    return std::nullopt;
  }
}

void liu::shader::active() const {
  for (size_t i = 0; i < app.command_buffers.size(); i++) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(app.command_buffers[i], &beginInfo) != VK_SUCCESS) {
      throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = app.render_pass;
    renderPassInfo.framebuffer = app.swap_chain_frame_buffers[i];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = app.swap_chain_extent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(app.command_buffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(app.command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline);

    vkCmdDraw(app.command_buffers[i], 3, 1, 0, 0);
  }
}

void liu::shader::inactive() const {
  for (auto command_buffer : app.command_buffers) {
    vkCmdEndRenderPass(command_buffer);

    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer!");
    }
  }
}
