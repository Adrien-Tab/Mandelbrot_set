#pragma once

#include <iostream>
#include <string>
#include <mutex>
#include <condition_variable>

namespace GlobalConfig {
namespace {
struct ConfigData {
  uint32_t iter_max;
  uint32_t color_scheme;
  double zoom_level;
  double center_x;
  double center_y;
  double fract_width;
  double fract_height;
  bool need_redraw;
  bool window_resized;
};

// Multi threads render
extern std::condition_variable g_render_condition;
extern std::mutex g_render_mutex;

// Multi threads accessible
extern ConfigData g_config_data;
extern std::mutex g_config_mutex;
}  // namespace

void parse_from_argv(int argc, char* argv[]);
void change_iter_max(int delta_iter_max);
void change_zoom(double change_factor);
void move_center(double x, double y);
void set_fractal_dim(double fract_width, double fract_height);
void set_window_resized(bool resized);
void switch_color_scheme();
uint32_t get_iter_max();
double get_zoom_level();
uint32_t get_color_scheme();
bool is_window_resized();
std::pair<double, double> get_center();
std::pair<double, double> get_fractal_dim();
void print_configuration();

// Notify fo redraw
void need_redraw();
void wait_to_draw();

}  // namespace GlobalConfig

namespace LogInfo {

namespace {
struct LogData {
  float fractal_time_ms;
  float display_time_ms;
};

extern LogData g_log_data;
extern std::mutex g_log_mutex;
}  // namespace

void print_log();
void set_fractal_time(double time_ms);
void set_display_time_ms(double time_ms);

}  // namespace LogInfo