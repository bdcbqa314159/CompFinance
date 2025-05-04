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
#ifndef CHOLDC_HPP
#define CHOLDC_HPP

#include "matrix.hpp"

template <class T> void choldc(const matrix<T> &in, matrix<T> &out) {
  int n = in.rows();
  T sum;

  fill(out.begin(), out.end(), T(0.0));

  for (int i = 0; i < n; ++i) {
    auto *ai = in[i];
    auto *pi = out[i];
    for (int j = 0; j <= i; ++j) {
      auto *aj = in[j];
      auto *pj = out[j];
      sum = ai[j];
      for (int k = 0; k < j; ++k) {
        sum -= pi[k] * pj[k];
      }
      if (i == j) {
        if (sum < -1.0e-15) {
          throw runtime_error("choldc : matrix not positive definite");
        }
        if (sum < 1.0e-15)
          sum = 0.0;
        pi[i] = sqrt(sum);
      } else {
        if (fabs(pj[j]) < 1.0e-15) {
          pi[j] = 0.0;
        } else {
          pi[j] = sum / pj[j];
        }
      }
    }
  }
}

#endif // CHOLDC_HPP