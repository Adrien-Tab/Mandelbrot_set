#pragma once

#include <cstdint>

#ifdef USE_HIP
#include <hip/hip_complex.h>
using Complex = hipDoubleComplex;
#else
#include <complex>
using Complex = std::complex<double>;
#endif

#include "core/window_dim.hpp"
#include "core/window_utils.hpp"

// For each pixel of the screen compute the number of step to
// escape from sphere of radius 2 and center 0.
void mandelbrot(const WindowDim<uint32_t> screen, const WindowDim<double> fract,
                uint32_t* escape_step, uint32_t iter_max);
