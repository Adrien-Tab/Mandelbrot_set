#include "glfw_utils.hpp"
#include "utils.hpp"
#include <iostream>

namespace GLFWUtils {

int initialize() {
  glfwSetErrorCallback([](int code, const char* const message) {
    std::cerr << "A glfw error encountered: " << message << "(" << code << ")\n";
  });

  // GLFW_PLATFORM_X11;
  // GLFW_PLATFORM_WAYLAND;
  // glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
  if (glfwInit() != GLFW_TRUE) {
    std::cerr << "failed to initialize GLFW\n";
    std::exit(1);
  }

  return 0;
}

void finalize() { glfwTerminate(); }

GLFWwindow* create_window(const int initial_width, const int initial_height) {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
  glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

  GLFWwindow* window =
      glfwCreateWindow(initial_width, initial_height, "Mandelbrot set", nullptr, nullptr);
  glfwSetWindowMonitor(window, nullptr, 300, 300, initial_width, initial_height,
                       GLFW_DONT_CARE);
  if (window == nullptr) {
    std::cerr << "Failed to create GLFW window\n";
    std::exit(1);
  }

  glfwMakeContextCurrent(window);

  // Initialize GLAD to load OpenGL function pointers
  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    std::cerr << "Failed to load OpenGL function pointers with GLAD" << std::endl;
    std::exit(1);
  }

  return window;
}

void print_configuration() {
  std::cout << std::endl;
  std::cout << "GL and GLFW configurations:\n"
            << "  OpenGL Version  :   " << glGetString(GL_VERSION) << "\n"
            << "  OpenGL Renderer :   " << glGetString(GL_RENDERER) << "\n"
            << "  OpenGL Vendor   :   " << glGetString(GL_VENDOR) << "\n"
            << "  OpenGL SHL      :   " << glGetString(GL_SHADING_LANGUAGE_VERSION)
            << "\n"
            << "  GLFW Version    :   " << glfwGetVersionString() << "\n"
            << "  GLFW Platform   :   " << get_glfw_platform_name() << "\n";
  std::cout << std::endl;
}

const char* get_glfw_platform_name() {
  int platform = glfwGetPlatform();
  switch (platform) {
    case GLFW_PLATFORM_WIN32:
      return "Win32";
    case GLFW_PLATFORM_X11:
      return "X11";
    case GLFW_PLATFORM_WAYLAND:
      return "Wayland";
    default:
      return "Unknown";
  }
}

}  // namespace GLFWUtils
