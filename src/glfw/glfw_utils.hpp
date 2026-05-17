#pragma once

#include "glad.h"

#include <GLFW/glfw3.h>

#include <memory>

namespace GLFWUtils {

struct GLFWwindowDeleter {
  void operator()(GLFWwindow* window) const noexcept {
    if (window != nullptr) {
      glfwDestroyWindow(window);
    }
  }
};

using UniqueWindow = std::unique_ptr<GLFWwindow, GLFWwindowDeleter>;

UniqueWindow create_window(const int initial_width, const int initial_height);
int initialize();
void finalize();
const char* get_glfw_platform_name();
void print_configuration();

}  // namespace GLFWUtils
