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

  int device_count = 0;
  HIP_CHECK(hipGetDeviceCount(&device_count));

  if (device_count == 0) {
    std::cerr << "No HIP devices found" << std::endl;
  } else {
    std::cout << "Number of HIP devices: " << device_count << std::endl;
  }
  print_device_prop(0);
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

void print_device_prop(int deviceIndex) {
  hipDeviceProp_t device_prop;
  HIP_CHECK(hipGetDeviceProperties(&device_prop, deviceIndex));

  std::cout.width(26);
  std::cout << std::left << "Device name: " << device_prop.name << std::endl;
  std::cout.width(26);
  std::cout << std::left << "GCN arch name: " << device_prop.gcnArchName << std::endl;

  int runtime_version = 0;
  HIP_CHECK(hipRuntimeGetVersion(&runtime_version));
  int version_major = runtime_version / 10000000;
  int version_minor = (runtime_version - version_major * 10000000) / 100000;
  int version_patch = runtime_version - version_major * 10000000 - version_minor * 100000;
  std::cout.width(26);
  std::cout << std::left << "HIP runtime version: " << version_major << "."
            << version_minor << "." << version_patch << std::endl;

  HIP_CHECK(hipDriverGetVersion(&runtime_version));
  version_major = runtime_version / 10000000;
  version_minor = (runtime_version - version_major * 10000000) / 100000;
  version_patch = runtime_version - version_major * 10000000 - version_minor * 100000;
  std::cout.width(26);
  std::cout << std::left << "HIP driver version: " << version_major << "."
            << version_minor << "." << version_patch << std::endl;

  std::cout << std::endl;
  std::cout.width(32);
  std::cout << std::left
            << "Total global memory: " << device_prop.totalGlobalMem / (1024 * 1024)
            << " MB" << std::endl;
  std::cout.width(32);
  std::cout << std::left
            << "Has warp vote: " << (device_prop.arch.hasWarpVote ? "Yes" : "No")
            << std::endl;
  std::cout.width(32);
  std::cout << std::left << "Warp size: " << device_prop.warpSize << std::endl;
  std::cout.width(32);
  std::cout << std::left << "Multiprocessor count: " << device_prop.multiProcessorCount
            << std::endl;
  std::cout.width(32);
  std::cout << std::left << "Max threads per block: " << device_prop.maxThreadsPerBlock
            << std::endl;
  std::cout.width(32);
  std::cout << std::left << "Max threads per multiprocessor: "
            << device_prop.maxThreadsPerMultiProcessor << std::endl;

  std::cout << std::endl;
  std::cout.width(30);
  std::cout << std::left
            << "Registers per multiprocessor: " << device_prop.regsPerMultiprocessor
            << std::endl;
  std::cout.width(30);
  std::cout << std::left << "Registers per block: " << device_prop.regsPerBlock
            << std::endl;
}
