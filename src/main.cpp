#include <cstdint>
#include <iostream>
#include <thread>

#include "glfw/glfw_utils.hpp"

#include "core/window_dim.hpp"
#include "core/window_utils.hpp"
#include "fractal/mandelbrot.hpp"
#include "gl/gl_mandelbrot.hpp"

#ifndef USE_HIP
import core.global_config;
#else
#include "core/global_config.hpp"
#endif

template <typename T>
  requires(std::is_floating_point_v<T>)
void OGL_handle(WindowDim<T> fract) {
  auto window = GLFWUtils::create_window(1200, 800);
  GLFWUtils::print_configuration();

  handle_event(window.get());

  handle_render(window.get(), &fract);
}

int main(int argc, char* argv[]) {
  GlobalConfig::parse_from_argv(argc, argv);
  GlobalConfig::print_configuration();

  // WindowDim : where we focus in the fractal
  WindowDim<double> fract(-2.2, 1.2, -1.7, 1.7);

  GlobalConfig::set_fractal_dim(fract.width(), fract.height());

  GLFWUtils::initialize();
  OGL_handle(fract);
  GLFWUtils::finalize();

  return 0;
}
