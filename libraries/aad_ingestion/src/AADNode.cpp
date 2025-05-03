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

#include "AADNode.hpp"
#include <algorithm>

using std::all_of;

size_t Node::numAdj = 1;

Node::Node(const size_t N) : n(N) {}
double &Node::adjoint() { return mAdjoint; }
double &Node::adjoint(const size_t n) { return pAdjoints[n]; }

void Node::propagateOne() {
  if (!n || !mAdjoint)
    return;

  for (size_t i = 0; i < n; ++i) {
    *(pAdjPtrs[i]) += mAdjoint * pDerivatives[i];
  }
}

void Node::propagateAll() {
  if (!n ||
      all_of(pAdjoints, pAdjoints + numAdj, [](const double &x) { return !x; }))
    return;

  for (size_t i = 0; i < n; ++i) {
    double *adjPtrs = pAdjPtrs[i], ders = pDerivatives[i];

    for (size_t j = 0; j < numAdj; ++j) {

      adjPtrs[j] += mAdjoint * ders * pAdjoints[j];
    }
  }
}