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
#ifndef AADTAPE_HPP
#define AADTAPE_HPP

#include "AADNode.hpp"
#include "blocklist.hpp"

constexpr size_t BLOCKSIZE = 16384;
constexpr size_t ADJSIZE = 32768;
constexpr size_t DATASIZE = 65536;

class Tape {
  static bool multi;

  blocklist<double, ADJSIZE> myAdjointsMulti;

  blocklist<double, DATASIZE> myDers;
  blocklist<double *, DATASIZE> myArgPtrs;

  blocklist<Node, BLOCKSIZE> myNodes;

  char myPad[64];

  friend auto setNumResultsForAAD(const bool, const size_t);
  friend struct numResultsResetterForAAD;
  friend class Number;

public:
  template <size_t N> Node *recordNode();

  void resetAdjoints();

  void clear();

  void rewind();

  void mark();

  void rewindToMark();

  using iterator = blocklist<Node, BLOCKSIZE>::iterator;

  inline auto begin() { return myNodes.begin(); }

  inline auto end() { return myNodes.end(); }

  inline auto markIt() { return myNodes.mark(); }

  inline auto find(Node *node) { return myNodes.find(node); }
};

#endif // AADTAPE_HPP