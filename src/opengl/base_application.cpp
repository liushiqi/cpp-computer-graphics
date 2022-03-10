#include <base_application.hpp>
#include <glad/gl.h>
#include <logger.hpp>

void APIENTRY gl_debug_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, [[maybe_unused]] GLsizei length,
                                        const GLchar *message, [[maybe_unused]] const void *userParam) {
  std::string source_string;
  switch (source) {
  case GL_DEBUG_SOURCE_API:
    source_string = "API";
    break;
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    source_string = "Window System";
    break;
  case GL_DEBUG_SOURCE_SHADER_COMPILER:
    source_string = "Shader Compiler";
    break;
  case GL_DEBUG_SOURCE_THIRD_PARTY:
    source_string = "Third Party";
    break;
  case GL_DEBUG_SOURCE_APPLICATION:
    source_string = "Application";
    break;
  case GL_DEBUG_SOURCE_OTHER:
    source_string = "Other";
    break;
  default:
    warn("Source: unknown");
    break;
  }

  std::string severity_string;
  switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH:
    severity_string = "high";
    break;
  case GL_DEBUG_SEVERITY_MEDIUM:
    severity_string = "medium";
    break;
  case GL_DEBUG_SEVERITY_LOW:
    break;
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    severity_string = "notification";
    break;
  default:
    severity_string = "unknown";
    break;
  }

  std::string type_string;
  switch (type) {
  case GL_DEBUG_TYPE_ERROR:
    type_string = "Error";
    break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    type_string = "Deprecated Behaviour";
    break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    type_string = "Undefined Behaviour";
    break;
  case GL_DEBUG_TYPE_PORTABILITY:
    type_string = "Portability";
    break;
  case GL_DEBUG_TYPE_PERFORMANCE:
    type_string = "Performance";
    break;
  case GL_DEBUG_TYPE_MARKER:
    type_string = "Marker";
    break;
  case GL_DEBUG_TYPE_PUSH_GROUP:
    type_string = "Push Group";
    break;
  case GL_DEBUG_TYPE_POP_GROUP:
    type_string = "Pop Group";
    break;
  case GL_DEBUG_TYPE_OTHER:
    type_string = "Other";
    break;
  default:
    type_string = "unknown";
    break;
  }

  std::string information =
      fmt::format("OpenGL {} severity message ({}) from {} of type {}: {}", severity_string, id, source_string, type_string, message);
  switch (type) {
  case GL_DEBUG_TYPE_ERROR:
    error("{}", information);
    break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
  case GL_DEBUG_TYPE_PORTABILITY:
  case GL_DEBUG_TYPE_PERFORMANCE:
    warn("{}", information);
    break;
  case GL_DEBUG_TYPE_MARKER:
  case GL_DEBUG_TYPE_PUSH_GROUP:
  case GL_DEBUG_TYPE_POP_GROUP:
  case GL_DEBUG_TYPE_OTHER:
    info("{}", information);
    break;
  default:
    fatal("{}", information);
    break;
  }
}

void liu::init_context() {
  int result = glfwInit();
  assert_log(result == GLFW_TRUE, "Failed to initialize GLFW");

  int glfw_version_major, glfw_version_minor, glfw_version_rev;
  glfwGetVersion(&glfw_version_major, &glfw_version_minor, &glfw_version_rev);
  info("GLFW initialization succeeded, version: {}.{}.{}", glfw_version_major, glfw_version_minor, glfw_version_rev);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
}

void liu::clean_context() { glfwTerminate(); }

void liu::base_application::init_context() {
  int result = gladLoadGL((GLADloadfunc)glfwGetProcAddress);
  assert_log(result == VK_SUCCESS, "Failed to load OpenGL");

  GLint flags;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
  if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
    glDebugMessageCallback(gl_debug_message_callback, nullptr);
    info("Debug enabled.");
  }
}

void liu::base_application::clean_context() { info("Clean context finished."); }
