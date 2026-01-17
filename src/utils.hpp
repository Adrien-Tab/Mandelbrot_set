#pragma once

#define INLINE_FUNCTION inline

#ifdef USE_HIP
#define DEVICE_HOST_INLINE INLINE_FUNCTION __host__ __device__
#else
#define DEVICE_HOST_INLINE INLINE_FUNCTION
#endif

#define DEVICE_INLINE_FUNCTION __device__ __forceinline__

#include <iostream>

#define DEBUG(msg)                                    \
  {                                                   \
    std::cout << __LINE__ << ":" << msg << std::endl; \
  }

#ifdef USE_HIP
#include <hip/hip_runtime.h>
#define HIP_CHECK(condition)                                             \
  {                                                                      \
    hipError_t err = condition;                                          \
    if (err != hipSuccess) {                                             \
      std::cerr << __FILE__ << ":" << __LINE__ << "\t"                   \
                << "HIP error: " << hipGetErrorString(err) << std::endl; \
    }                                                                    \
  }
#endif  // USE_HIP

#define GL_CHECK()                                                      \
  {                                                                     \
    GLenum err = glGetError();                                          \
    if (err != GL_NO_ERROR) {                                           \
      std::cerr << __FILE__ << ":" << __LINE__ << "\t"                  \
                << "GL error: ";                                        \
      switch (err) {                                                    \
        case GL_INVALID_ENUM:                                           \
          std::cerr << "GL_INVALID_ENUM" << std::endl;                  \
          break;                                                        \
        case GL_INVALID_VALUE:                                          \
          std::cerr << "GL_INVALID_VALUE" << std::endl;                 \
          break;                                                        \
        case GL_INVALID_OPERATION:                                      \
          std::cerr << "GL_INVALID_OPERATION" << std::endl;             \
          break;                                                        \
        case GL_INVALID_FRAMEBUFFER_OPERATION:                          \
          std::cerr << "GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl; \
          break;                                                        \
        case GL_OUT_OF_MEMORY:                                          \
          std::cerr << "GL_OUT_OF_MEMORY" << std::endl;                 \
          break;                                                        \
        case GL_STACK_UNDERFLOW:                                        \
          std::cerr << "GL_STACK_UNDERFLOW" << std::endl;               \
          break;                                                        \
        case GL_STACK_OVERFLOW:                                         \
          std::cerr << "GL_STACK_OVERFLOW" << std::endl;                \
          break;                                                        \
        default:                                                        \
          std::cerr << "Uknown Error" << std::endl;                     \
      }                                                                 \
    }                                                                   \
  }

#ifdef USE_HIP
inline void print_device_prop(int deviceIndex) {
  hipDeviceProp_t device_prop;
  HIP_CHECK(hipGetDeviceProperties(&device_prop, deviceIndex));

  std::cout.width(26);
  std::cout << std::left << "Device name: " << device_prop.name << std::endl;
  std::cout.width(26);
  std::cout << std::left << "GCN arch name: " << device_prop.gcnArchName << std::endl;

  int runtime_version = 0;
  HIP_CHECK(hipRuntimeGetVersion(&runtime_version));
  int version_major = runtime_version / 10000000;
  int version_minor = (runtime_version - version_major * 10000000) / 100000;
  int version_patch = runtime_version - version_major * 10000000 - version_minor * 100000;
  std::cout.width(26);
  std::cout << std::left << "HIP runtime version: " << version_major << "."
            << version_minor << "." << version_patch << std::endl;

  HIP_CHECK(hipDriverGetVersion(&runtime_version));
  version_major = runtime_version / 10000000;
  version_minor = (runtime_version - version_major * 10000000) / 100000;
  version_patch = runtime_version - version_major * 10000000 - version_minor * 100000;
  std::cout.width(26);
  std::cout << std::left << "HIP driver version: " << version_major << "."
            << version_minor << "." << version_patch << std::endl;

  std::cout << std::endl;
  std::cout.width(32);
  std::cout << std::left
            << "Total global memory: " << device_prop.totalGlobalMem / (1024 * 1024)
            << " MB" << std::endl;
  std::cout.width(32);
  std::cout << std::left
            << "Has warp vote: " << (device_prop.arch.hasWarpVote ? "Yes" : "No")
            << std::endl;
  std::cout.width(32);
  std::cout << std::left << "Warp size: " << device_prop.warpSize << std::endl;
  std::cout.width(32);
  std::cout << std::left << "Multiprocessor count: " << device_prop.multiProcessorCount
            << std::endl;
  std::cout.width(32);
  std::cout << std::left << "Max threads per block: " << device_prop.maxThreadsPerBlock
            << std::endl;
  std::cout.width(32);
  std::cout << std::left << "Max threads per multiprocessor: "
            << device_prop.maxThreadsPerMultiProcessor << std::endl;

  std::cout << std::endl;
  std::cout.width(30);
  std::cout << std::left
            << "Registers per multiprocessor: " << device_prop.regsPerMultiprocessor
            << std::endl;
  std::cout.width(30);
  std::cout << std::left << "Registers per block: " << device_prop.regsPerBlock
            << std::endl;
}
#endif  // USE_HIP
