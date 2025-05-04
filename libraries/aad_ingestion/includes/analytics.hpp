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
#ifndef ANALYTICS_HPP
#define ANALYTICS_HPP

#include "gaussians.hpp"

//  Black and Scholes formula, templated

//  Price
template <class T, class U, class V, class W>
inline T blackScholes(const U spot, const V strike, const T vol, const W mat) {
  const auto std = vol * sqrt(mat);
  if (std <= EPS)
    return max<T>(T(0.0), T(spot - strike));
  const auto d2 = log(spot / strike) / std - 0.5 * std;
  const auto d1 = d2 + std;
  return spot * normalCdf(d1) - strike * normalCdf(d2);
}

//  Implied vol, untemplated
double blackScholesIvol(const double spot, const double strike,
                        const double prem, const double mat);

//  Merton, templated
template <class T, class U, class V, class W, class X>
inline T merton(const U spot, const V strike, const T vol, const W mat,
                const X intens, const X meanJmp, const X stdJmp) {
  const auto varJmp = stdJmp * stdJmp;
  const auto mv2 = meanJmp + 0.5 * varJmp;
  const auto comp = intens * (exp(mv2) - 1);
  const auto var = vol * vol;
  const auto intensT = intens * mat;

  unsigned fact = 1;
  X iT = 1.0;
  const size_t cut = 10;
  T result = 0.0;
  for (size_t n = 0; n < cut; ++n) {
    const auto s = spot * exp(n * mv2 - comp * mat);
    const auto v = sqrt(var + n * varJmp / mat);
    const auto prob = exp(-intensT) * iT / fact;
    result += prob * blackScholes(s, strike, v, mat);
    fact *= n + 1;
    iT *= intensT;
  }

  return result;
}

//	Up and out call in Black-Scholes, untemplated

double BlackScholesKO(const double spot, const double rate, const double div,
                      const double strike, const double barrier,
                      const double mat, const double vol);

#endif // ANALYTICS_HPP