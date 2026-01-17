#pragma once

#include <cstdint>

#include "window_dim.hpp"

namespace WindowUtils {

void zoom(const double window_ratio, const double x0, const double x1, const double y0,
          const double y1, WindowDim<double>* fract);

void zoom(const double x0, const double x1, const double y0, const double y1,
          WindowDim<double>* fract);

void zoom(const double center_x, const double center_y, const double new_zoom_level,
          WindowDim<double>* fract);

void move(const double dx, const double dy, WindowDim<double>* fract);

void adjust_ratio(const WindowDim<uint32_t> screen, WindowDim<double>* fract);

}  // namespace WindowUtils
