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