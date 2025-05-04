// Code modified by Bernardo Cohen
// Original code by Antoine Savine
/*
Written by Antoine Savine in 2018

This code is the strict IP of Antoine Savine

License to use and alter this code for personal and commercial applications
is freely granted to any person or company who purchased a copy of the book

Modern Computational Finance: AAD and Parallel Simulations
Antoine Savine
Wiley, 2018

As long as this comment is preserved at the top of the file
*/

#pragma once
#ifndef INTERP_HPP
#define INTERP_HPP

#include "matrix.hpp"
#include <algorithm>
#include <vector>
using namespace std;

template <bool smoothStep = false, class ITX, class ITY, class T>
inline auto interp(ITX xBegin, ITX xEnd, ITY yBegin, ITY yEnd, const T &x0)
    -> remove_reference_t<decltype(*yBegin)> {

  auto it = upper_bound(xBegin, xEnd, x0);

  if (it == xEnd)
    return *(yEnd - 1);
  if (it == xBegin)
    return *yBegin;

  size_t n = distance(xBegin, it) - 1;
  auto x1 = xBegin[n];
  auto y1 = yBegin[n];
  auto x2 = xBegin[n + 1];
  auto y2 = yBegin[n + 1];

  auto t = (x0 - x1) / (x2 - x1);

  if constexpr (smoothStep) {
    return y1 + (y2 - y1) * t * t * (3.0 - 2 * t);
  } else {
    return y1 + (y2 - y1) * t;
  }
}

template <bool smoothStep = false, class T, class U, class V, class W, class X>
inline V interp2D(const vector<T> &x, const vector<U> &y, const matrix<V> &z,
                  const W &x0, const X &y0) {
  const size_t n = x.size();
  const size_t m = y.size();

  auto it = upper_bound(x.begin(), x.end(), x0);
  const size_t n2 = distance(x.begin(), it);

  if (n2 == n)
    return interp<smoothStep>(y.begin(), y.end(), z[n2 - 1], z[n2 - 1] + m, y0);
  if (n2 == 0)
    return interp<smoothStep>(y.begin(), y.end(), z[0], z[0] + m, y0);

  const size_t n1 = n2 - 1;
  auto x1 = x[n1];
  auto x2 = x[n2];
  auto z1 = interp<smoothStep>(y.begin(), y.end(), z[n1], z[n1] + m, y0);
  auto z2 = interp<smoothStep>(y.begin(), y.end(), z[n2], z[n2] + m, y0);

  auto t = (x0 - x1) / (x2 - x1);
  if constexpr (smoothStep) {
    return z1 + (z2 - z1) * t * t * (3.0 - 2 * t);
  } else {
    return z1 + (z2 - z1) * t;
    ;
  }
}

#endif // INTERP_HPP