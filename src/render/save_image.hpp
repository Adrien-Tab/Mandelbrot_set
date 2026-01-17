#pragma once

#include <cstdint>
#include <string>

#include "core/window_dim.hpp"

// Used to get generate file names
std::string now_to_string();
std::string pos_to_string(double x, double y, double zoom_level);

void save_image(WindowDim<uint32_t>& scr, uint32_t* colors, uint32_t iter_max,
                const char* fname);
