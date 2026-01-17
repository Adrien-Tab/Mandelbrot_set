#ifdef USE_SEQUENTIAL

#include "fractal/mandelbrot.hpp"

template <typename F, typename T = Complex>
concept FractalFunction = std::invocable<F, T, T> && requires(F f, T a, T b) {
  { f(a, b) } -> std::same_as<T>;
};

// Convert a pixel coordinate to the complex domain
Complex scale(const WindowDim<uint32_t>& screen, const WindowDim<double>& fr,
              Complex c) noexcept {
  // Avoid repeated calculations by storing width and height ratios
  const double x_ratio = fr.width() / static_cast<double>(screen.width());
  const double y_ratio = fr.height() / static_cast<double>(screen.height());

  return Complex(c.real() * x_ratio + fr.x_min(), c.imag() * y_ratio + fr.y_min());
}

// Check if a point is in the set or escapes to infinity, return the number if iterations
template <FractalFunction Fractal_t>
uint32_t escape(Complex c, uint32_t iter_max, Fractal_t func) {
  Complex z(0);
  uint32_t iter = 0;

  while (abs(z) < 2.0 && iter < iter_max) {
    z = func(z, c);
    iter++;
  }

  return iter;
}

// Loop over each pixel from our image and check if the points associated with this pixel
// escape to infinity
template <FractalFunction Fractal_t>
void get_number_iterations(const WindowDim<uint32_t>& screen,
                           const WindowDim<double>& fract, uint32_t iter_max,
                           uint32_t* escape_step, Fractal_t func) {
  int k = 0;
  // iterate through pixel
  for (uint32_t i = screen.y_min(); i < screen.y_max(); ++i) {
    for (uint32_t j = screen.x_min(); j < screen.x_max(); ++j) {
      Complex c((double)j, (double)i);
      c              = scale(screen, fract, c);
      escape_step[k] = escape(c, iter_max, func);
      k++;
    }
  }
}

void mandelbrot(const WindowDim<uint32_t> screen, const WindowDim<double> fract,
                uint32_t* escape_step, uint32_t iter_max) {
  // The function used to calculate the fractal
  constexpr auto func = [](const Complex z, const Complex c) noexcept -> Complex {
    return z * z + c;
  };

  get_number_iterations(screen, fract, iter_max, escape_step, func);
  return;
}

#endif  // USE_SEQUENTIAL
