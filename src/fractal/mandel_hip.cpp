#ifdef USE_HIP

#include "fractal/mandelbrot.hpp"
#include "utils.hpp"

#include <hip/hip_runtime.h>
#include <iostream>

template <typename F, typename T = Complex>
concept FractalFunction = requires(F f, T a, T b) {
  { f(a, b) } -> std::same_as<T>;
};

// From screen coordinate to fract complex coord
DEVICE_INLINE_FUNCTION Complex scale(const WindowDim<uint32_t>& screen,
                                     const WindowDim<double>& fract, Complex c) {
  const double x_ratio = fract.width() / static_cast<double>(screen.width());
  const double y_ratio = fract.height() / static_cast<double>(screen.height());
  return make_hipDoubleComplex(hipCreal(c) * x_ratio + fract.x_min(),
                               hipCimag(c) * y_ratio + fract.y_min());
}

template <FractalFunction Fractal_t>
DEVICE_INLINE_FUNCTION uint32_t escape(Complex c, uint32_t iter_max, Fractal_t func) {
  Complex z     = make_hipDoubleComplex(0, 0);
  uint32_t iter = 0;
  while (hipCabs(z) < 2.0 && iter < iter_max) {
    z = func(z, c);
    iter++;
  }

  return iter;
}

// Note: Don't use ref in kernel
template <FractalFunction Fractal_t>
__global__ void get_number_iterations(const WindowDim<uint32_t> screen,
                                      const WindowDim<double> fract, uint32_t iter_max,
                                      uint32_t* escape_step, Fractal_t func) {
  const uint32_t col    = blockIdx.x * blockDim.x + threadIdx.x;
  const uint32_t row    = blockIdx.y * blockDim.y + threadIdx.y;
  const uint32_t width  = screen.width();
  const uint32_t height = screen.height();

  if (col < width && row < height) {
    Complex c           = make_hipDoubleComplex((double)col, (double)row);
    c                   = scale(screen, fract, c);
    uint32_t num_step   = escape(c, iter_max, func);
    uint32_t global_idx = row * width + col;
    __syncthreads();
    escape_step[global_idx] = num_step;
  }
  return;
}

void mandelbrot(const WindowDim<uint32_t> screen, const WindowDim<double> fract,
                uint32_t* escape_step, uint32_t iter_max) {
  constexpr auto func = [] __device__(const Complex z, const Complex c) -> Complex {
    return hipCfma(z, z, c);
  };

  dim3 block_size(16, 8, 1);
  int grid_x = (screen.width() + block_size.x - 1) / block_size.x;
  int grid_y = (screen.height() + block_size.y - 1) / block_size.y;
  dim3 grid_size(grid_x, grid_y, 1);
  uint32_t* d_escape_step;
  const size_t alloc_size = screen.size() * sizeof(uint32_t);

  HIP_CHECK(hipSetDevice(0));
  HIP_CHECK(hipMalloc((void**)&d_escape_step, alloc_size));
  HIP_CHECK(hipMemset(d_escape_step, 0, alloc_size));

  hipLaunchKernelGGL(HIP_KERNEL_NAME(get_number_iterations<decltype(func)>), grid_size,
                     block_size, 0, hipStreamDefault, screen, fract, iter_max,
                     d_escape_step, func);

  HIP_CHECK(hipMemcpy(escape_step, d_escape_step, alloc_size, hipMemcpyDeviceToHost));
  HIP_CHECK(hipDeviceSynchronize());
  HIP_CHECK(hipGetLastError());
  HIP_CHECK(hipFree(d_escape_step));
  return;
}

#endif
