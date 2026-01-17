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
void sfml_handle(WindowDim<T> fract) {
  GLFWUtils::initialize();
  GLFWwindow* const window = GLFWUtils::create_window(1200, 800);
  GLFWUtils::print_configuration();

  handle_event(window);

  handle_render(window, &fract);
  /*
  window.setActive(false);

  // std::thread julia_thread(&julia_handle);
  std::thread rendering_thread(&render_handle, &window, &fract);

  rendering_thread.join();
  // julia_thread.join();
  */
  GLFWUtils::finalize();
}

int main(int argc, char* argv[]) {
  GlobalConfig::parse_from_argv(argc, argv);
  GlobalConfig::print_configuration();

  // WindowDim : where we focus in the fractal
  WindowDim<double> fract(-2.2, 1.2, -1.7, 1.7);

  GlobalConfig::set_fractal_dim(fract.width(), fract.height());

  sfml_handle(fract);

  return 0;
}
