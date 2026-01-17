#include "color_scheme.hpp"

#include "core/global_config.hpp"

#include <functional>
#include <concepts>

template <typename F, typename T>
concept ColorSchemeFunction =
    std::invocable<F, T, T> && std::integral<T> && requires(F f, T n, T max) {
      { f(n, max) } -> std::convertible_to<typename ColorSchemes::RGB_t>;
    };

namespace ColorSchemes {

constexpr auto color_scheme_functions = std::make_tuple(
    &piecewise_linear, &scheme_1, &scheme_2, &scheme_3, &scheme_4, &scheme_5);

RGB_t get_color(uint32_t n, uint32_t iter_max) {
  uint32_t idx = GlobalConfig::get_color_scheme();
  switch (idx % std::tuple_size_v<decltype(color_scheme_functions)>) {
    case 0:
      return std::invoke(std::get<0>(color_scheme_functions), n, iter_max);
    case 1:
      return std::invoke(std::get<1>(color_scheme_functions), n, iter_max);
    case 2:
      return std::invoke(std::get<2>(color_scheme_functions), n, iter_max);
    case 3:
      return std::invoke(std::get<3>(color_scheme_functions), n, iter_max);
    case 4:
      return std::invoke(std::get<4>(color_scheme_functions), n, iter_max);
    case 5:
      return std::invoke(std::get<5>(color_scheme_functions), n, iter_max);
    default:
      return std::invoke(std::get<1>(color_scheme_functions), n, iter_max);
  }
}

RGB_t piecewise_linear(uint32_t n, uint32_t iter_max) {
  int N  = 256;  // colors per element
  int N3 = N * N * N;
  // map n on the 0..1 interval (real numbers)
  double t = (double)n / (double)iter_max;
  // expand n on the 0 .. 256^3 interval (integers)
  n = (int)(t * (double)N3);

  uint8_t b = n / (N * N);
  int nn    = n - b * N * N;
  uint8_t r = nn / N;
  uint8_t g = nn - r * N;
  return std::make_tuple(r, g, b);
}

RGB_t scheme_1(uint32_t n, uint32_t iter_max) {
  // map n on the 0..1 interval
  double t = (double)n / (double)iter_max;

  // Use smooth polynomials for r, g, b
  uint8_t r = (uint8_t)(9 * (1 - t) * t * t * t * 255);
  uint8_t g = (uint8_t)(15 * (1 - t) * (1 - t) * t * t * 255);
  uint8_t b = (uint8_t)(8.5 * (1 - t) * (1 - t) * (1 - t) * t * 255);
  return std::make_tuple(r, g, b);
}

RGB_t scheme_2(uint32_t n, uint32_t iter_max) {
  // map n on the 0..1 interval
  double t = (double)n / (double)iter_max;

  // Use smooth polynomials for r, g, b
  uint8_t b = (uint8_t)(9 * (1 - t) * t * t * t * 255);
  uint8_t g = (uint8_t)(15 * (1 - t) * (1 - t) * t * t * 255);
  uint8_t r = (uint8_t)(8.5 * (1 - t) * (1 - t) * (1 - t) * t * 255);
  return std::make_tuple(r, g, b);
}

RGB_t scheme_3(uint32_t n, uint32_t iter_max) {
  // map n on the 0..1 interval
  double t = (double)n / (double)iter_max;

  // Use smooth polynomials for r, g, b
  uint8_t r = (uint8_t)(9 * (1 - t) * t * t * t * 255);
  uint8_t g = (uint8_t)(8 * (1 - t) * (1 - t) * t * t * 255);
  uint8_t b = (uint8_t)(9 * (1 - t) * (1 - t) * (1 - t) * t * 255);
  return std::make_tuple(r, g, b);
}

RGB_t scheme_4(uint32_t n, uint32_t iter_max) {
  // map n on the 0..1 interval
  double t = (double)n / (double)iter_max;

  uint8_t k = (uint8_t)(t * 255);
  return std::make_tuple(k, k, k);
}

RGB_t scheme_5(uint32_t n, uint32_t iter_max) {
  // map n on the 0..1 interval
  double t = (double)n / (double)iter_max;

  uint8_t k = (uint8_t)((1 - t) * 255);
  return std::make_tuple(k, k, k);
}

}  // namespace ColorSchemes

static_assert(ColorSchemeFunction<decltype(ColorSchemes::piecewise_linear), uint32_t>);
static_assert(ColorSchemeFunction<decltype(ColorSchemes::scheme_1), uint32_t>);
static_assert(ColorSchemeFunction<decltype(ColorSchemes::scheme_2), uint32_t>);
static_assert(ColorSchemeFunction<decltype(ColorSchemes::scheme_3), uint32_t>);
static_assert(ColorSchemeFunction<decltype(ColorSchemes::scheme_4), uint32_t>);
