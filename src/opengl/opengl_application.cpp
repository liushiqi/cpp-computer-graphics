#include <application.hpp>
#include <glad/gl.h>
#include <logger.hpp>

void APIENTRY gl_debug_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, [[maybe_unused]] GLsizei length,
                                        const GLchar *message, [[maybe_unused]] const void *userParam) {
  warn("Debug message ({}): {}", id, message);

  switch (source) {
  case GL_DEBUG_SOURCE_API:
    warn("Source: API");
    break;
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    warn("Source: Window System");
    break;
  case GL_DEBUG_SOURCE_SHADER_COMPILER:
    warn("Source: Shader Compiler");
    break;
  case GL_DEBUG_SOURCE_THIRD_PARTY:
    warn("Source: Third Party");
    break;
  case GL_DEBUG_SOURCE_APPLICATION:
    warn("Source: Application");
    break;
  case GL_DEBUG_SOURCE_OTHER:
    warn("Source: Other");
    break;
  default:
    warn("Source: unknown");
    break;
  }

  switch (type) {
  case GL_DEBUG_TYPE_ERROR:
    warn("Type: Error");
    break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    warn("Type: Deprecated Behaviour");
    break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    warn("Type: Undefined Behaviour");
    break;
  case GL_DEBUG_TYPE_PORTABILITY:
    warn("Type: Portability");
    break;
  case GL_DEBUG_TYPE_PERFORMANCE:
    warn("Type: Performance");
    break;
  case GL_DEBUG_TYPE_MARKER:
    warn("Type: Marker");
    break;
  case GL_DEBUG_TYPE_PUSH_GROUP:
    warn("Type: Push Group");
    break;
  case GL_DEBUG_TYPE_POP_GROUP:
    warn("Type: Pop Group");
    break;
  case GL_DEBUG_TYPE_OTHER:
    warn("Type: Other");
    break;
  default:
    warn("Type: unknown");
    break;
  }

  switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH:
    warn("Severity: high");
    break;
  case GL_DEBUG_SEVERITY_MEDIUM:
    warn("Severity: medium");
    break;
  case GL_DEBUG_SEVERITY_LOW:
    warn("Severity: low");
    break;
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    warn("Severity: notification");
    break;
  default:
    warn("Severity: unknown");
    break;
  }
}

void liu::init_context() {
  if (!glfwInit()) {
    fatal("GLFW initialization failed.");
    throw std::runtime_error("GLFW initialization failed.");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
}

void liu::clean_context() {
  glfwTerminate();
  gladLoaderUnloadGL();
}

void liu::application::init_context() {
  if (!gladLoaderLoadGL()) {
    fatal("Failed to load OpenGL.");
    exit(1);
  }

  GLint flags;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
  if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
    glDebugMessageCallback(gl_debug_message_callback, nullptr);
    info("Debug enabled.");
  }
}

void liu::application::cleanup_context() { info("Clean context finished."); }
