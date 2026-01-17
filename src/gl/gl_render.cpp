#include "gl_mandelbrot.hpp"
#include "gl/gl_shader.hpp"

#include <iostream>
#include <thread>

#include "core/window_utils.hpp"
#include "core/global_config.hpp"
#include "fractal/mandelbrot.hpp"
#include "render/color_scheme.hpp"

void handle_render(GLFWwindow* const window, WindowDim<double>* fract) {
  glEnable(GL_TEXTURE_2D);
  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClearDepth(1.f);
  GL_CHECK();

  constexpr float vertices[] = {1.0f, 1.0f,  0.0f, 1.0f,  1.0f,  1.0f, -1.0f,
                                0.0f, 1.0f,  0.0f, -1.0f, -1.0f, 0.0f, 0.0f,
                                0.0f, -1.0f, 1.0f, 0.0f,  0.0f,  1.0f};

  constexpr unsigned int indices[] = {0, 1, 3, 1, 2, 3};

  // Vertex
  GLuint VAO, VBO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // Position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  // Texture attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  GL_CHECK();

  // Shaders
  GLuint vertex_shader, fragment_shader;
  GLUtils::loadShader(vertex_shader, GL_VERTEX_SHADER, GLUtils::VertexShader);
  GLUtils::loadShader(fragment_shader, GL_FRAGMENT_SHADER, GLUtils::FragmentShader);
  GL_CHECK();

  GLuint shader_program = glCreateProgram();
  GLUtils::linkProgram(&shader_program, vertex_shader, fragment_shader);
  GL_CHECK();

  // Texture 2D
  GLuint texture_image = 0;
  int width            = 0;
  int height           = 0;

  glGenTextures(1, &texture_image);
  glBindTexture(GL_TEXTURE_2D, texture_image);

  // Texture parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glfwGetWindowSize(window, &width, &height);
  WindowDim<uint32_t> screen(width, height);

  GLubyte* texture_data = new GLubyte[screen.size() * 3];
  WindowUtils::adjust_ratio(screen, fract);
  GlobalConfig::set_fractal_dim(fract->width(), fract->height());
  uint32_t* escape_step = new uint32_t[screen.size()];

  int iter_max = GlobalConfig::get_iter_max();
  mandelbrot(screen, *fract, escape_step, iter_max);

  size_t k = 0;
  // Iterate each pixel
  for (const auto& _ : screen) {
    uint32_t n              = escape_step[k];
    auto [r, g, b]          = ColorSchemes::get_color(n, iter_max);
    texture_data[3 * k + 0] = r;
    texture_data[3 * k + 1] = g;
    texture_data[3 * k + 2] = b;
    k++;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,
               texture_data);
  GL_CHECK();

  unsigned int frame_counter = 0;
  while (!glfwWindowShouldClose(window)) {
    auto [center_x, center_y] = GlobalConfig::get_center();
    iter_max                  = GlobalConfig::get_iter_max();
    if (GlobalConfig::is_window_resized()) {
      glfwGetWindowSize(window, &width, &height);
      screen.reset(0, width, 0, height);

      delete[] escape_step;
      delete[] texture_data;
      escape_step  = new uint32_t[screen.size()];
      texture_data = new GLubyte[screen.size() * 3];
      std::cout << "Screen size : " << screen.size() << std::endl;
      WindowUtils::adjust_ratio(screen, fract);
      GlobalConfig::set_fractal_dim(fract->width(), fract->height());

      GlobalConfig::set_window_resized(false);
    }

    std::tie(center_x, center_y) = GlobalConfig::get_center();
    auto zoom                    = GlobalConfig::get_zoom_level();
    GlobalConfig::set_fractal_dim(fract->width(), fract->height());
    WindowUtils::zoom(center_x, center_y, zoom, fract);

    auto fractal_start = std::chrono::steady_clock::now();
    mandelbrot(screen, *fract, escape_step, iter_max);
    LogInfo::set_fractal_time(std::chrono::duration<float, std::milli>(
                                  std::chrono::steady_clock::now() - fractal_start)
                                  .count());

    auto display_start = std::chrono::steady_clock::now();

    k = 0;
    // Iterate each pixel
    for (const auto& _ : screen) {
      uint32_t n              = escape_step[k];
      auto [r, g, b]          = ColorSchemes::get_color(n, iter_max);
      texture_data[3 * k + 0] = b;
      texture_data[3 * k + 1] = g;
      texture_data[3 * k + 2] = r;
      k++;
    }

    glBindTexture(GL_TEXTURE_2D, texture_image);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE,
                    texture_data);

    glUseProgram(shader_program);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    LogInfo::set_display_time_ms(std::chrono::duration<float, std::milli>(
                                     std::chrono::steady_clock::now() - display_start)
                                     .count());
    frame_counter++;
    if (frame_counter % 30 == 0) {
      LogInfo::print_log();
    }
    glfwWaitEvents();
    glClear(GL_COLOR_BUFFER_BIT);
    // std::cout << "New event !" << std::endl;
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteProgram(shader_program);
  glDeleteTextures(1, &texture_image);
  delete[] texture_data;
  delete[] escape_step;
}

/*
void updateTextureFromColorArray(sf::Texture &texture, const sf::Color *colorArray) {
  texture.update(reinterpret_cast<const sf::Uint8 *>(colorArray));
  return;
}

void makeTexture(const WindowDim<uint32_t> &screen, sf::Texture &texture) {
  bool success = texture.create(screen.width(), screen.height());
  if (!success) {
    std::cerr << "Error " << __FILE__ << ":" << __LINE__ << std::endl;
  }
  return;
}

void set_texture(const WindowDim<uint32_t> &screen, uint32_t *escape_step,
                 uint32_t iter_max, sf::Texture &texture, sf::Color *pixelArray) {
  size_t k = 0;
  for (unsigned int y = 0; y < screen.height(); y++) {
    for (unsigned int x = 0; x < screen.width(); x++) {
      uint32_t n      = escape_step[k];
      auto [r, g, b]  = ColorSchemes::get_color(n, iter_max);
      pixelArray[k].r = r;
      pixelArray[k].g = g;
      pixelArray[k].b = b;
      pixelArray[k].a = 255;
      k++;
    }
  }

  updateTextureFromColorArray(texture, pixelArray);
  return;
}

void render_handle(sf::RenderWindow *renderWindow, WindowDim<double> *fract) {
  renderWindow->setActive(true);
  sf::Vector2u dim = renderWindow->getSize();
  WindowDim<uint32_t> screen(0, dim.x, 0, dim.y);

  auto [center_x, center_y] = GlobalConfig::get_center();
  WindowUtils::zoom(center_x, center_y, GlobalConfig::get_zoom_level(), fract);

  WindowUtils::adjust_ratio(screen, fract);
  GlobalConfig::set_fractal_dim(fract->width(), fract->height());

  uint32_t *escape_step = new uint32_t[screen.size()];
  sf::Color *pixelArray = new sf::Color[screen.size()];

  sf::Sprite fractal_sprite;
  sf::Texture fractal_texture;

  makeTexture(screen, fractal_texture);
  fractal_sprite.setTexture(fractal_texture);

  while (renderWindow->isOpen()) {
    GlobalConfig::wait_to_draw();

    if (GlobalConfig::is_window_resized()) {
      dim    = renderWindow->getSize();
      screen = WindowDim<uint32_t>(0, dim.x, 0, dim.y);

      delete[] escape_step;
      delete[] pixelArray;
      escape_step = new uint32_t[screen.size()];
      pixelArray  = new sf::Color[screen.size()];

      WindowUtils::adjust_ratio(screen, fract);
      GlobalConfig::set_fractal_dim(fract->width(), fract->height());

      makeTexture(screen, fractal_texture);
      fractal_sprite.setTexture(fractal_texture, true);

      GlobalConfig::set_window_resized(false);
    }

    renderWindow->clear(sf::Color::Black);

    std::tie(center_x, center_y) = GlobalConfig::get_center();
    auto zoom                    = GlobalConfig::get_zoom_level();
    GlobalConfig::set_fractal_dim(fract->width(), fract->height());
    WindowUtils::zoom(center_x, center_y, zoom, fract);

    auto fractal_start = std::chrono::steady_clock::now();
    mandelbrot(screen, *fract, escape_step, GlobalConfig::get_iter_max());

    LogInfo::set_fractal_time(std::chrono::duration<float, std::milli>(
                                  std::chrono::steady_clock::now() - fractal_start)
                                  .count());

    auto display_start = std::chrono::steady_clock::now();
    set_texture(screen, escape_step, GlobalConfig::get_iter_max(), fractal_texture,
                pixelArray);

    renderWindow->draw(fractal_sprite);
    renderWindow->display();

    LogInfo::set_display_time_ms(std::chrono::duration<float, std::milli>(
                                     std::chrono::steady_clock::now() - display_start)
                                     .count());
    LogInfo::print_log();
  }

  delete[] escape_step;
  delete[] pixelArray;
}

*/
