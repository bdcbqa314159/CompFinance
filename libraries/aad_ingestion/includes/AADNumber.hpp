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
#ifndef NUMBER_HPP
#define NUMBER_HPP

#include "AADTape.hpp"
#include "gaussians.hpp"

class Number {

  double myValue;
  Node *myNode;

  template <size_t N> void createNode() { myNode = tape->recordNode<N>(); }

  Node &node() const {

#ifdef _DEBUG

    auto it = tape->find(myNode);

    if (it == tape->end()) {
      throw runtime_error("Put a breakpoint here");
    }

#endif
    return const_cast<Node &>(*myNode);
  }

  double &derivative() { return myNode->pDerivatives[0]; }
  double &lDer() { return myNode->pDerivatives[0]; }
  double &rDer() { return myNode->pDerivatives[1]; }

  double *&adjPtr() { return myNode->pAdjPtrs[0]; }
  double *&leftAdj() { return myNode->pAdjPtrs[0]; }
  double *&rightAdj() { return myNode->pAdjPtrs[1]; }

  Number(Node &arg, const double val) : myValue(val) {
    createNode<1>();

    myNode->pAdjPtrs[0] = Tape::multi ? arg.pAdjoints : &arg.mAdjoint;
  }

  Number(Node &lhs, Node &rhs, const double val) : myValue(val) {
    createNode<2>();

    if (Tape::multi) {
      myNode->pAdjPtrs[0] = lhs.pAdjoints;
      myNode->pAdjPtrs[1] = rhs.pAdjoints;
    } else {
      myNode->pAdjPtrs[0] = &lhs.mAdjoint;
      myNode->pAdjPtrs[1] = &rhs.mAdjoint;
    }
  }

public:
  static thread_local Tape *tape;

  Number() {}

  explicit Number(const double val) : myValue(val) { createNode<0>(); }

  Number &operator=(const double val) {
    myValue = val;
    createNode<0>();

    return *this;
  }

  void putOnTape() { createNode<0>(); }

  explicit operator double &() { return myValue; }
  explicit operator double() const { return myValue; }

  double &value() { return myValue; }
  double value() const { return myValue; }
  double &adjoint() { return myNode->adjoint(); }
  double adjoint() const { return myNode->adjoint(); }
  double &adjoint(const size_t n) { return myNode->adjoint(n); }
  double adjoint(const size_t n) const { return myNode->adjoint(n); }
  void resetAdjoints() { tape->resetAdjoints(); }

  static void propagateAdjoints(Tape::iterator propagateFrom,
                                Tape::iterator propagateTo) {
    auto it = propagateFrom;
    while (it != propagateTo) {
      it->propagateOne();
      --it;
    }
    it->propagateOne();
  }

  void propagateAdjoints(Tape::iterator propagateTo) {

    adjoint() = 1.0;
    auto propagateFrom = tape->find(myNode);
    propagateAdjoints(propagateFrom, propagateTo);
  }

  void propagateToStart() { propagateAdjoints(tape->begin()); }
  void propagateToMark() { propagateAdjoints(tape->markIt()); }

  static void propagateMarkToStart() {
    propagateAdjoints(prev(tape->markIt()), tape->begin());
  }

  static void propagateAdjointsMulti(Tape::iterator propagateFrom,
                                     Tape::iterator propagateTo) {
    auto it = propagateFrom;
    while (it != propagateTo) {
      it->propagateAll();
      --it;
    }
    it->propagateAll();
  }

  inline friend Number operator+(const Number &lhs, const Number &rhs) {
    const double e = lhs.value() + rhs.value();
    Number result(lhs.node(), rhs.node(), e);
    result.lDer() = 1.0;
    result.rDer() = 1.0;

    return result;
  }
  inline friend Number operator+(const Number &lhs, const double &rhs) {
    const double e = lhs.value() + rhs;
    Number result(lhs.node(), e);
    result.derivative() = 1.0;

    return result;
  }
  inline friend Number operator+(const double &lhs, const Number &rhs) {
    return rhs + lhs;
  }

  inline friend Number operator-(const Number &lhs, const Number &rhs) {
    const double e = lhs.value() - rhs.value();
    Number result(lhs.node(), rhs.node(), e);
    result.lDer() = 1.0;
    result.rDer() = -1.0;

    return result;
  }

  inline friend Number operator-(const Number &lhs, const double &rhs) {
    const double e = lhs.value() - rhs;
    Number result(lhs.node(), e);
    result.derivative() = 1.0;

    return result;
  }
  inline friend Number operator-(const double &lhs, const Number &rhs) {
    const double e = lhs - rhs.value();
    Number result(rhs.node(), e);
    result.derivative() = -1.0;

    return result;
  }

  inline friend Number operator*(const Number &lhs, const Number &rhs) {
    const double e = lhs.value() * rhs.value();
    Number result(lhs.node(), rhs.node(), e);
    result.lDer() = rhs.value();
    result.rDer() = lhs.value();

    return result;
  }
  inline friend Number operator*(const Number &lhs, const double &rhs) {
    const double e = lhs.value() * rhs;
    Number result(lhs.node(), e);
    result.derivative() = rhs;

    return result;
  }
  inline friend Number operator*(const double &lhs, const Number &rhs) {
    return rhs * lhs;
  }

  inline friend Number operator/(const Number &lhs, const Number &rhs) {
    const double e = lhs.value() / rhs.value();
    Number result(lhs.node(), rhs.node(), e);
    const double invRhs = 1.0 / rhs.value();
    result.lDer() = invRhs;
    result.rDer() = -lhs.value() * invRhs * invRhs;

    return result;
  }
  inline friend Number operator/(const Number &lhs, const double &rhs) {
    const double e = lhs.value() / rhs;
    Number result(lhs.node(), e);
    result.derivative() = 1.0 / rhs;

    return result;
  }
  inline friend Number operator/(const double &lhs, const Number &rhs) {
    const double e = lhs / rhs.value();
    Number result(rhs.node(), e);
    result.derivative() = -lhs / rhs.value() / rhs.value();

    return result;
  }

  inline friend Number pow(const Number &lhs, const Number &rhs) {
    const double e = pow(lhs.value(), rhs.value());
    Number result(lhs.node(), rhs.node(), e);
    result.lDer() = rhs.value() * e / lhs.value();
    result.rDer() = log(lhs.value()) * e;

    return result;
  }
  inline friend Number pow(const Number &lhs, const double &rhs) {
    const double e = pow(lhs.value(), rhs);
    Number result(lhs.node(), e);
    result.derivative() = rhs * e / lhs.value();

    return result;
  }
  inline friend Number pow(const double &lhs, const Number &rhs) {
    const double e = pow(lhs, rhs.value());
    Number result(rhs.node(), e);
    result.derivative() = log(lhs) * e;

    return result;
  }

  inline friend Number max(const Number &lhs, const Number &rhs) {
    const bool lmax = lhs.value() > rhs.value();
    Number result(lhs.node(), rhs.node(), lmax ? lhs.value() : rhs.value());
    if (lmax) {
      result.lDer() = 1.0;
      result.rDer() = 0.0;
    } else {
      result.lDer() = 0.0;
      result.rDer() = 1.0;
    }

    return result;
  }
  inline friend Number max(const Number &lhs, const double &rhs) {
    const bool lmax = lhs.value() > rhs;
    Number result(lhs.node(), lmax ? lhs.value() : rhs);
    result.derivative() = lmax ? 1.0 : 0.0;

    return result;
  }
  inline friend Number max(const double &lhs, const Number &rhs) {
    const bool rmax = rhs.value() > lhs;
    Number result(rhs.node(), rmax ? rhs.value() : lhs);
    result.derivative() = rmax ? 1.0 : 0.0;

    return result;
  }

  inline friend Number min(const Number &lhs, const Number &rhs) {
    const bool lmin = lhs.value() < rhs.value();
    Number result(lhs.node(), rhs.node(), lmin ? lhs.value() : rhs.value());
    if (lmin) {
      result.lDer() = 1.0;
      result.rDer() = 0.0;
    } else {
      result.lDer() = 0.0;
      result.rDer() = 1.0;
    }

    return result;
  }
  inline friend Number min(const Number &lhs, const double &rhs) {
    const bool lmin = lhs.value() < rhs;
    Number result(lhs.node(), lmin ? lhs.value() : rhs);
    result.derivative() = lmin ? 1.0 : 0.0;

    return result;
  }

  inline friend Number min(const double &lhs, const Number &rhs) {
    const bool rmin = rhs.value() < lhs;

    Number result(rhs.node(), rmin ? rhs.value() : lhs);
    result.derivative() = rmin ? 1.0 : 0.0;

    return result;
  }

  Number &operator+=(const Number &arg) {
    *this = *this + arg;
    return *this;
  }
  Number &operator+=(const double &arg) {
    *this = *this + arg;
    return *this;
  }

  Number &operator-=(const Number &arg) {
    *this = *this - arg;
    return *this;
  }
  Number &operator-=(const double &arg) {
    *this = *this - arg;
    return *this;
  }

  Number &operator*=(const Number &arg) {
    *this = *this * arg;
    return *this;
  }
  Number &operator*=(const double &arg) {
    *this = *this * arg;
    return *this;
  }

  Number &operator/=(const Number &arg) {
    *this = *this / arg;
    return *this;
  }
  Number &operator/=(const double &arg) {
    *this = *this / arg;
    return *this;
  }

  Number operator-() const { return 0.0 - *this; }
  Number operator+() const { return *this; }

  inline friend Number exp(const Number &arg) {
    const double e = exp(arg.value());
    Number result(arg.node(), e);
    result.derivative() = e;

    return result;
  }

  inline friend Number log(const Number &arg) {
    const double e = log(arg.value());
    Number result(arg.node(), e);
    result.derivative() = 1.0 / arg.value();
    return result;
  }

  inline friend Number sqrt(const Number &arg) {
    const double e = sqrt(arg.value());
    Number result(arg.node(), e);
    result.derivative() = 0.5 / e;
    return result;
  }

  inline friend Number fabs(const Number &arg) {
    const double e = fabs(arg.value());
    Number result(arg.node(), e);
    result.derivative() = arg.value() > 0.0 ? 1.0 : -1.0;
    return result;
  }

  inline friend Number normalDens(const Number &arg) {
    const double e = normalDens(arg.value());
    Number result(arg.node(), e);
    result.derivative() = -arg.value() * e;

    return result;
  }

  inline friend Number normalCdf(const Number &arg) {
    const double e = normalCdf(arg.value());
    Number result(arg.node(), e);
    result.derivative() = normalDens(arg.value());

    return result;
  }

  inline friend bool operator==(const Number &lhs, const Number &rhs) {
    return lhs.value() == rhs.value();
  }
  inline friend bool operator==(const Number &lhs, const double &rhs) {
    return lhs.value() == rhs;
  }
  inline friend bool operator==(const double &lhs, const Number &rhs) {
    return lhs == rhs.value();
  }

  inline friend bool operator!=(const Number &lhs, const Number &rhs) {
    return lhs.value() != rhs.value();
  }
  inline friend bool operator!=(const Number &lhs, const double &rhs) {
    return lhs.value() != rhs;
  }
  inline friend bool operator!=(const double &lhs, const Number &rhs) {
    return lhs != rhs.value();
  }

  inline friend bool operator<(const Number &lhs, const Number &rhs) {
    return lhs.value() < rhs.value();
  }
  inline friend bool operator<(const Number &lhs, const double &rhs) {
    return lhs.value() < rhs;
  }
  inline friend bool operator<(const double &lhs, const Number &rhs) {
    return lhs < rhs.value();
  }

  inline friend bool operator>(const Number &lhs, const Number &rhs) {
    return lhs.value() > rhs.value();
  }
  inline friend bool operator>(const Number &lhs, const double &rhs) {
    return lhs.value() > rhs;
  }
  inline friend bool operator>(const double &lhs, const Number &rhs) {
    return lhs > rhs.value();
  }

  inline friend bool operator<=(const Number &lhs, const Number &rhs) {
    return lhs.value() <= rhs.value();
  }
  inline friend bool operator<=(const Number &lhs, const double &rhs) {
    return lhs.value() <= rhs;
  }
  inline friend bool operator<=(const double &lhs, const Number &rhs) {
    return lhs <= rhs.value();
  }

  inline friend bool operator>=(const Number &lhs, const Number &rhs) {
    return lhs.value() >= rhs.value();
  }
  inline friend bool operator>=(const Number &lhs, const double &rhs) {
    return lhs.value() >= rhs;
  }
  inline friend bool operator>=(const double &lhs, const Number &rhs) {
    return lhs >= rhs.value();
  }
};

#endif // NUMBER_HPP