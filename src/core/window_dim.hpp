#pragma once

#include "utils.hpp"

template <typename T>
struct WindowDim {
  T _x_min, _x_max, _y_min, _y_max;
  double _zoom_level;

  WindowDim(T x_min, T x_max, T y_min, T y_max)
      : _x_min(x_min), _x_max(x_max), _y_min(y_min), _y_max(y_max), _zoom_level(1.0) {}

  WindowDim(T width, T height)
      : _x_min(0), _x_max(width), _y_min(0), _y_max(height), _zoom_level(1.0) {}

  // Utility functions for getting the size, width and height of the window
  DEVICE_HOST_INLINE T size() const { return (width() * height()); }

  DEVICE_HOST_INLINE T width() const { return (_x_max - _x_min); }

  DEVICE_HOST_INLINE T height() const { return (_y_max - _y_min); }

  DEVICE_HOST_INLINE T get_zoom_level() const { return (_zoom_level); }

  // Getters and setters for the window elements
  DEVICE_HOST_INLINE T x_min() const { return _x_min; }

  void x_min(T x_min) { _x_min = x_min; }

  DEVICE_HOST_INLINE T x_max() const { return _x_max; }

  void x_max(T x_max) { _x_max = x_max; }

  DEVICE_HOST_INLINE T y_min() const { return _y_min; }

  void y_min(T y_min) { _y_min = y_min; }

  DEVICE_HOST_INLINE T y_max() const { return _y_max; }

  void y_max(T y_max) { _y_max = y_max; }

  // Reset all values
  void reset(T x_min, T x_max, T y_min, T y_max) {
    _x_min = x_min;
    _x_max = x_max;
    _y_min = y_min;
    _y_max = y_max;
  }

  void reset(T x_min, T x_max, T y_min, T y_max, double zoom_level) {
    _x_min      = x_min;
    _x_max      = x_max;
    _y_min      = y_min;
    _y_max      = y_max;
    _zoom_level = zoom_level;
  }

  // Iterate each pixel (row first)
  struct Iterator {
   private:
    T x;
    T y;
    T x_max;
    T y_max;

   public:
    Iterator(T _x, T _y, T _x_max, T _y_max)
        : x(_x), y(_y), x_max(_x_max), y_max(_y_max) {};

    std::pair<T, T> operator*() const { return std::pair<T, T>{x, y}; }

    Iterator& operator++() {
      if (this->x < x_max - 1) {
        this->x++;
      } else {
        this->x = 0;
        this->y++;
      }
      return *this;
    }

    bool operator==(const Iterator& other_it) const {
      return this->x == other_it.x && this->y == other_it.y;
    }
  };

  Iterator begin() { return Iterator(_x_min, _y_min, _x_max, _y_max); }

  Iterator end() { return Iterator(0, _y_max, _x_max, _y_max); }
};
