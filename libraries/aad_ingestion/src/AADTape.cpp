#include "AADTape.hpp"

bool Tape::multi = false;

template <size_t N> Node *Tape::recordNode() {

  Node *node = myNodes.emplace_back(N);

  if (multi) {
    node->pAdjoints = myAdjointsMulti.emplace_back_multi(Node::numAdj);
    fill(node->pAdjoints, node->pAdjoints + Node::numAdj, 0.0);
  }

  if constexpr (N > 0) {
    node->pDerivatives = myDers.emplace_back_multi<N>();
    node->pAdjPtrs = myArgPtrs.emplace_back_multi<N>();
  }

  return node;
}

void Tape::resetAdjoints() {
  if (multi) {
    myAdjointsMulti.myMemset();
  } else {
    for (Node &node : myNodes) {
      node.mAdjoint = 0;
    }
  }
}

void Tape::clear() {
  myAdjointsMulti.clear();
  myDers.clear();
  myArgPtrs.clear();
  myNodes.clear();
}

void Tape::rewind() {

#ifdef _DEBUG

  clear();

#else

  if (multi) {
    myAdjointsMulti.rewind();
  }
  myDers.rewind();
  myArgPtrs.rewind();
  myNodes.rewind();

#endif
}

void Tape::mark() {
  if (multi) {
    myAdjointsMulti.setmark();
  }
  myDers.setmark();
  myArgPtrs.setmark();
  myNodes.setmark();
}

void Tape::rewindToMark() {
  if (multi) {
    myAdjointsMulti.rewind_to_mark();
  }
  myDers.rewind_to_mark();
  myArgPtrs.rewind_to_mark();
  myNodes.rewind_to_mark();
}