#include "global_config.hpp"

#include <iomanip>

namespace GlobalConfig {
namespace {
ConfigData g_config_data{.iter_max       = 200,
                         .color_scheme   = 1,
                         .zoom_level     = 1.0,
                         .center_x       = 0.0,
                         .center_y       = 0.0,
                         .fract_width    = 0.0,
                         .fract_height   = 0.0,
                         .need_redraw    = true,
                         .window_resized = false};
std::mutex g_config_mutex;

std::condition_variable g_render_condition;
std::mutex g_render_mutex;
}  // namespace

#define TRY_CATCH_DEFAULT_VALUE(var, expr, value)        \
  try {                                                  \
    var = expr;                                          \
  } catch (const std::exception& e) {                    \
    std::cerr << __FILE__ << ":" << __LINE__ << "\t"     \
              << "Exception: \"" << e.what() << "\"\t"   \
              << "Default value set: " << value << "\n"; \
    var = value;                                         \
  }

void parse_from_argv(int argc, char* argv[]) {
  std::lock_guard<std::mutex> lock(g_config_mutex);

  for (int i = 1; i < argc; i++) {
    std::string_view arg(argv[i]);
    if (arg == "-h" || arg == "--help") {
      std::cout << "Mandelbrot \n"
                << "Options: \n"
                << " [-i <int>]            iter max\n"
                << " [-c <float> <float>]  center \n"
                << " [-z <float>]          zoom level \n"
                << " [-h, --help]          for help" << std::endl;
      exit(0);
    } else if (arg == "-i" && i + 1 < argc) {
      TRY_CATCH_DEFAULT_VALUE(g_config_data.iter_max, std::stoi(argv[++i]), 500)
    } else if (arg == "-c" && i + 2 < argc) {
      TRY_CATCH_DEFAULT_VALUE(g_config_data.center_x, std::stod(argv[++i]), 0.0)
      TRY_CATCH_DEFAULT_VALUE(g_config_data.center_y, std::stod(argv[++i]), 0.0)
    } else if (arg == "-z" && i + 1 < argc) {
      TRY_CATCH_DEFAULT_VALUE(g_config_data.zoom_level, std::stod(argv[++i]), 0.0)
    }
  }
}

void change_iter_max(int delta_iter_max) {
  std::lock_guard lock(g_config_mutex);
  if (g_config_data.iter_max + delta_iter_max > 1) {
    g_config_data.iter_max += delta_iter_max;
  }
}

void change_zoom(double zoom_factor) {
  std::lock_guard lock(g_config_mutex);
  g_config_data.zoom_level *= zoom_factor;
}

void set_fractal_dim(double new_width, double new_height) {
  std::lock_guard lock(g_config_mutex);
  g_config_data.fract_width  = new_width;
  g_config_data.fract_height = new_height;
}

void set_window_resized(bool resized) {
  std::lock_guard lock(g_config_mutex);
  g_config_data.window_resized = resized;
}

void switch_color_scheme() {
  std::lock_guard lock(g_config_mutex);
  g_config_data.color_scheme++;
}

bool is_window_resized() { return g_config_data.window_resized; }

std::pair<double, double> get_fractal_dim() {
  return std::make_pair(g_config_data.fract_width, g_config_data.fract_height);
}

void need_redraw() {
  {
    std::lock_guard lock(g_render_mutex);
    g_config_data.need_redraw = true;
  }
  g_render_condition.notify_all();
}

void wait_to_draw() {
  std::unique_lock lock(g_render_mutex);
  g_render_condition.wait(lock, []() { return g_config_data.need_redraw; });
  g_config_data.need_redraw = false;
}

void move_center(double dx, double dy) {
  std::lock_guard lock(g_config_mutex);
  g_config_data.center_x += dx;
  g_config_data.center_y += dy;
}

// getters
uint32_t get_iter_max() { return g_config_data.iter_max; }

double get_zoom_level() { return g_config_data.zoom_level; }

std::pair<double, double> get_center() {
  return std::make_pair(g_config_data.center_x, g_config_data.center_y);
}

uint32_t get_color_scheme() { return g_config_data.color_scheme; }

void print_configuration() {
  std::cout << "Current Configuration:\n"
            << "  Max iterations: " << g_config_data.iter_max << "\n"
            << "  Zoom level: " << g_config_data.zoom_level << "\n"
            << "  Center: (" << g_config_data.center_x << ", " << g_config_data.center_y
            << ")\n";
}

}  // namespace GlobalConfig

namespace LogInfo {
namespace {
LogData g_log_data{.fractal_time_ms = 0.0, .display_time_ms = 0.0};
std::mutex g_log_mutex;
}  // namespace

void print_log() {
  std::lock_guard<std::mutex> lock(g_log_mutex);
  std::cout << std::scientific << std::setprecision(6);
  std::cout << "Log Information:\n"
            << "  Center:       ( " << GlobalConfig::get_center().first << ", "
            << GlobalConfig::get_center().second << ")\n"
            << "  Zoom level:     " << GlobalConfig::get_zoom_level() << "\n"
            << std::defaultfloat << "  Iterations max: " << GlobalConfig::get_iter_max()
            << "\n\n"
            << "  Fractal time: " << g_log_data.fractal_time_ms << " ms\n"
            << "  Display time: " << g_log_data.display_time_ms << " ms\n";

  if (!std::cout) {
    std::cerr << "Error: Failed to write log information to standard output.\n";
  }
}

void set_fractal_time(double time_ms) {
  std::lock_guard<std::mutex> lock(g_log_mutex);
  g_log_data.fractal_time_ms = time_ms;
}

void set_display_time_ms(double time_ms) {
  std::lock_guard<std::mutex> lock(g_log_mutex);
  g_log_data.display_time_ms = time_ms;
}

}  // namespace LogInfo