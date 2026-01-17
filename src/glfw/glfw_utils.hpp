#pragma once

#include "glad.h"

#include <GLFW/glfw3.h>

namespace GLFWUtils {

GLFWwindow* create_window(const int initial_width, const int initial_height);
int initialize();
void finalize();
const char* get_glfw_platform_name();
void print_configuration();

}  // namespace GLFWUtils
