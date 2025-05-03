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
#ifndef AADNODE_HPP
#define AADNODE_HPP

#include <algorithm>
using std::all_of;

class Node {
  friend class Tape;
  friend class Number;
  friend auto setNumResultsForAAD(const bool, const size_t);
  friend struct numResultsResetterForAAD;

  double mAdjoint = 0.;
  double *pAdjoints = nullptr;
  double *pDerivatives = nullptr;
  double **pAdjPtrs = nullptr;

  static size_t numAdj; // initialize later
  const size_t n = 0;

public:
  Node(const size_t = 0);
  double &adjoint();
  double &adjoint(const size_t n);

  void propagateOne();

  void propagateAll();
};

#endif // AADNODE_HPP