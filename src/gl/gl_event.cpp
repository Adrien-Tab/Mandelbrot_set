#include "gl_mandelbrot.hpp"

#include <cstdint>
#include <thread>

#include "core/window_utils.hpp"
#include "fractal/mandelbrot.hpp"
#include "render/save_image.hpp"

#ifndef USE_HIP
import core.global_config;
#else
#include "core/global_config.hpp"
#endif

static void key_callback(GLFWwindow* window, int key, [[maybe_unused]] int scancode,
                         int action, [[maybe_unused]] int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
    glfwTerminate();
  }
  if (key == GLFW_KEY_TAB && action == GLFW_RELEASE) {
    GlobalConfig::switch_color_scheme();
  }
  if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
    GlobalConfig::change_iter_max(2);
  }
  if (key == GLFW_KEY_E && action == GLFW_PRESS) {
    GlobalConfig::change_iter_max(-2);
  }
  const double moveSpeed = 0.1 / GlobalConfig::get_zoom_level();
  if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
    GlobalConfig::move_center(-moveSpeed, 0);
  }
  if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
    GlobalConfig::move_center(moveSpeed, 0);
  }
  if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
    GlobalConfig::move_center(0, moveSpeed);
  }
  if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
    GlobalConfig::move_center(0, -moveSpeed);
  }
  if (action == GLFW_REPEAT) {
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
      GlobalConfig::change_iter_max(2);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
      GlobalConfig::change_iter_max(-2);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
      GlobalConfig::move_center(-moveSpeed, 0);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
      GlobalConfig::move_center(moveSpeed, 0);
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
      GlobalConfig::move_center(0, moveSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
      GlobalConfig::move_center(0, -moveSpeed);
    }
  }
}

static void scroll_callback(GLFWwindow* window, double, double yoffset) {
  double xpos = 0.0;
  double ypos = 0.0;
  int width   = 0;
  int height  = 0;
  glfwGetWindowSize(window, &width, &height);
  glfwGetCursorPos(window, &xpos, &ypos);

  auto [frac_width, frac_height] = GlobalConfig::get_fractal_dim();
  double zoomFactor              = yoffset > 0 ? 1.2 : 1 / 1.2;

  double ratio_x = frac_width / static_cast<double>(width);
  double ratio_y = frac_height / static_cast<double>(height);

  double delta_x = 0.5 * (xpos - static_cast<float>(width) * 0.5) * ratio_x;
  double delta_y = -0.5 * (ypos - static_cast<float>(height) * 0.5) * ratio_y;

  GlobalConfig::change_zoom(zoomFactor);
  GlobalConfig::move_center(delta_x, delta_y);
}

static void resize_callback(GLFWwindow* const, const int width, const int height) {
  glViewport(0, 0, width, height);
  GlobalConfig::set_window_resized(true);
}

void handle_event(GLFWwindow* const window) {
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_FALSE);
  glfwSetFramebufferSizeCallback(window, resize_callback);
  glfwSetKeyCallback(window, key_callback);
  glfwSetScrollCallback(window, scroll_callback);
}

/*
  void handle_event(sf::RenderWindow *const window) {
  sf::Event event;
  window->setActive(false);
  while (window->isOpen()) {
    while (window->pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        GlobalConfig::need_redraw();
        window->close();
        // Avoid deadlock
        return;
      }

      if (event.type == sf::Event::KeyPressed) {
        const double moveSpeed = 0.15 / GlobalConfig::get_zoom_level();

        if (event.key.code == sf::Keyboard::Left) {
          GlobalConfig::move_center(-moveSpeed, 0);
        }
        if (event.key.code == sf::Keyboard::Right) {
          GlobalConfig::move_center(moveSpeed, 0);
        }
        if (event.key.code == sf::Keyboard::Up) {
          GlobalConfig::move_center(0, -moveSpeed);
        }
        if (event.key.code == sf::Keyboard::Tab) {
          GlobalConfig::switch_color_scheme();
        }
        if (event.key.code == sf::Keyboard::Down) {
          GlobalConfig::move_center(0, moveSpeed);
        }
        if (event.key.code == sf::Keyboard::A) {
          GlobalConfig::change_iter_max(2);
        }
        if (event.key.code == sf::Keyboard::E) {
          GlobalConfig::change_iter_max(-2);
        }
        if (event.key.code == sf::Keyboard::P) {
          auto [frac_width, frac_height] = GlobalConfig::get_fractal_dim();
          auto [c_x, c_y]                = GlobalConfig::get_center();
          double zoom_level              = GlobalConfig::get_zoom_level();

          std::string position = pos_to_string(c_x, c_y, zoom_level);
          std::string filename = now_to_string() + position + ".png";

          WindowDim<uint32_t> screen(0, 2560, 0, 1440);
          WindowDim<double> fract(c_x - frac_width / 2, c_x + frac_width / 2,
                                  c_y - frac_height / 2, c_y + frac_height / 2);

          WindowUtils::adjust_ratio(screen, &fract);
          uint32_t *escape_step = new uint32_t[screen.size()];
          sf::Color *pixelArray = new sf::Color[screen.size()];

          mandelbrot(screen, fract, escape_step, GlobalConfig::get_iter_max());
          save_image(screen, escape_step, GlobalConfig::get_iter_max(), filename.c_str());
          delete[] escape_step;
          delete[] pixelArray;
        }
        GlobalConfig::need_redraw();
      }

      if (event.type == sf::Event::Resized) {
        sf::FloatRect visibleArea({0.f, 0.f},
                                  sf::Vector2f(event.size.width, event.size.height));
        window->setView(sf::View(visibleArea));
        GlobalConfig::set_window_resized(true);
        GlobalConfig::need_redraw();
      }

      if (event.type == sf::Event::MouseWheelScrolled) {
        if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
          sf::Vector2u windowSize    = window->getSize();
          sf::Vector2i mousePosition = sf::Mouse::getPosition(*window);
          sf::Vector2f mousePos(static_cast<float>(mousePosition.x),
                                static_cast<float>(mousePosition.y));

          auto [frac_width, frac_height] = GlobalConfig::get_fractal_dim();
          double zoomFactor = event.mouseWheelScroll.delta > 0 ? 1.2 : 1 / 1.2;

          double ratio_x = frac_width / static_cast<double>(windowSize.x);
          double ratio_y = frac_height / static_cast<double>(windowSize.y);

          double delta_x =
              0.5 * (mousePos.x - static_cast<float>(windowSize.x) * 0.5) * ratio_x;
          double delta_y =
              0.5 * (mousePos.y - static_cast<float>(windowSize.y) * 0.5) * ratio_y;

          GlobalConfig::change_zoom(zoomFactor);
          GlobalConfig::move_center(delta_x, delta_y);
          GlobalConfig::need_redraw();
        }
      }
    }
  }
}
*/
