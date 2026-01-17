#include "save_image.hpp"

#include <ctime>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

#include <FreeImage.h>

#include "core/window_dim.hpp"
#include "color_scheme.hpp"

#ifndef USE_HIP
import core.global_config;
#else
#include "core/global_config.hpp"
#endif

std::string now_to_string() {
  time_t t    = time(nullptr);
  tm* tm_time = localtime(&t);

  if (tm_time == nullptr) {
    std::cerr << "Error: localtime() \n";
    return std::string("DATE");
  }

  std::ostringstream string_stream;
  string_stream << tm_time->tm_year + 1900 << "-" << std::setfill('0') << std::setw(2)
                << tm_time->tm_mon + 1 << "-" << std::setfill('0') << std::setw(2)
                << tm_time->tm_mday << "_" << std::setfill('0') << std::setw(2)
                << tm_time->tm_hour << ":" << std::setfill('0') << std::setw(2)
                << tm_time->tm_min << ":" << std::setfill('0') << std::setw(2)
                << tm_time->tm_sec << std::setfill('0') << std::setw(2);

  std::string result = string_stream.str();
  return result;
}

std::string pos_to_string(double x, double y, double zoom_level) {
  std::ostringstream string_stream;
  string_stream << "(" << x << "," << y << ")x" << zoom_level;
  std::string result = string_stream.str();
  return result;
}

void save_image(WindowDim<uint32_t>& scr, uint32_t* escape_step, uint32_t iter_max,
                const char* fname) {
// active only for static linking
#ifdef FREEIMAGE_LIB
  FreeImage_Initialise();
#endif

  uint32_t width   = scr.width();
  uint32_t height  = scr.height();
  FIBITMAP* bitmap = FreeImage_Allocate(width, height, 32);  // RGBA

  int k = 0;
  ColorSchemes::RGB_t rgb;

  for (uint32_t i = scr.y_min(); i < scr.y_max(); ++i) {
    for (uint32_t j = scr.x_min(); j < scr.x_max(); ++j) {
      int n = escape_step[k];

      rgb = ColorSchemes::get_color(n, iter_max);

      RGBQUAD col;
      col.rgbRed      = std::get<0>(rgb);
      col.rgbGreen    = std::get<1>(rgb);
      col.rgbBlue     = std::get<2>(rgb);
      col.rgbReserved = 255;

      FreeImage_SetPixelColor(bitmap, j, i, &col);
      k++;
    }
  }

  // Save the image in a PNG file
  FreeImage_Save(FIF_PNG, bitmap, fname, PNG_DEFAULT);

  // Clean bitmap;
  FreeImage_Unload(bitmap);

// active only for static linking
#ifdef FREEIMAGE_LIB
  FreeImage_DeInitialise();
#endif
}
