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
#ifndef AADEXPR_HPP
#define AADEXPR_HPP

//  Implementation of AAD with expression templates
//  (AADET, chapter 15)

//  Defines expressions and the Number type

//  So we can instrument Gaussians like standard math functions
#include "AADTape.hpp"
#include "gaussians.hpp"
#include <algorithm>

template <class E> struct Expression {

  double value() const { return static_cast<const E *>(this)->value(); }
  explicit operator double() const { return value(); }
};

template <class LHS, class RHS, class OP>
class BinaryExpression : public Expression<BinaryExpression<LHS, RHS, OP>> {
  const double myValue;

  const LHS lhs;
  const RHS rhs;

public:
  explicit BinaryExpression(const Expression<LHS> &l, const Expression<RHS> &r)
      : myValue(OP::eval(l.value(), r.value())),
        lhs(static_cast<const LHS &>(l)), rhs(static_cast<const RHS &>(r)) {}

  double value() const { return myValue; }

  enum { numNumbers = LHS::numNumbers + RHS::numNumbers };

  template <size_t N, size_t n>
  void pushAdjoint(

      Node &exprNode,

      const double adjoint) const {
    if (LHS::numNumbers > 0) {
      lhs.template pushAdjoint<N, n>(
          exprNode,
          adjoint * OP::leftDerivative(lhs.value(), rhs.value(), value()));
    }

    if (RHS::numNumbers > 0) {
      rhs.template pushAdjoint<N, n + LHS::numNumbers>(
          exprNode,
          adjoint * OP::rightDerivative(lhs.value(), rhs.value(), value()));
    }
  }
};

struct OPMult {
  static const double eval(const double l, const double r) { return l * r; }

  static const double leftDerivative(const double l, const double r,
                                     const double v) {
    return r;
  }

  static const double rightDerivative(const double l, const double r,
                                      const double v) {
    return l;
  }
};

struct OPAdd {
  static const double eval(const double l, const double r) { return l + r; }

  static const double leftDerivative(const double l, const double r,
                                     const double v) {
    return 1.0;
  }

  static const double rightDerivative(const double l, const double r,
                                      const double v) {
    return 1.0;
  }
};

struct OPSub {
  static const double eval(const double l, const double r) { return l - r; }

  static const double leftDerivative(const double l, const double r,
                                     const double v) {
    return 1.0;
  }

  static const double rightDerivative(const double l, const double r,
                                      const double v) {
    return -1.0;
  }
};

struct OPDiv {
  static const double eval(const double l, const double r) { return l / r; }

  static const double leftDerivative(const double l, const double r,
                                     const double v) {
    return 1.0 / r;
  }

  static const double rightDerivative(const double l, const double r,
                                      const double v) {
    return -l / r / r;
  }
};

struct OPPow {
  static const double eval(const double l, const double r) { return pow(l, r); }

  static const double leftDerivative(const double l, const double r,
                                     const double v) {
    return r * v / l;
  }

  static const double rightDerivative(const double l, const double r,
                                      const double v) {
    return log(l) * v;
  }
};

struct OPMax {
  static const double eval(const double l, const double r) { return max(l, r); }

  static const double leftDerivative(const double l, const double r,
                                     const double v) {
    return l > r ? 1.0 : 0.0;
  }

  static const double rightDerivative(const double l, const double r,
                                      const double v) {
    return r > l ? 1.0 : 0.0;
  }
};

struct OPMin {
  static const double eval(const double l, const double r) { return min(l, r); }

  static const double leftDerivative(const double l, const double r,
                                     const double v) {
    return l < r ? 1.0 : 0.0;
  }

  static const double rightDerivative(const double l, const double r,
                                      const double v) {
    return r < l ? 1.0 : 0.0;
  }
};

template <class LHS, class RHS>
BinaryExpression<LHS, RHS, OPMult> operator*(const Expression<LHS> &lhs,
                                             const Expression<RHS> &rhs) {
  return BinaryExpression<LHS, RHS, OPMult>(lhs, rhs);
}

template <class LHS, class RHS>
BinaryExpression<LHS, RHS, OPAdd> operator+(const Expression<LHS> &lhs,
                                            const Expression<RHS> &rhs) {
  return BinaryExpression<LHS, RHS, OPAdd>(lhs, rhs);
}

template <class LHS, class RHS>
BinaryExpression<LHS, RHS, OPSub> operator-(const Expression<LHS> &lhs,
                                            const Expression<RHS> &rhs) {
  return BinaryExpression<LHS, RHS, OPSub>(lhs, rhs);
}

template <class LHS, class RHS>
BinaryExpression<LHS, RHS, OPDiv> operator/(const Expression<LHS> &lhs,
                                            const Expression<RHS> &rhs) {
  return BinaryExpression<LHS, RHS, OPDiv>(lhs, rhs);
}

template <class LHS, class RHS>
BinaryExpression<LHS, RHS, OPPow> pow(const Expression<LHS> &lhs,
                                      const Expression<RHS> &rhs) {
  return BinaryExpression<LHS, RHS, OPPow>(lhs, rhs);
}

template <class LHS, class RHS>
BinaryExpression<LHS, RHS, OPMax> max(const Expression<LHS> &lhs,
                                      const Expression<RHS> &rhs) {
  return BinaryExpression<LHS, RHS, OPMax>(lhs, rhs);
}

template <class LHS, class RHS>
BinaryExpression<LHS, RHS, OPMin> min(const Expression<LHS> &lhs,
                                      const Expression<RHS> &rhs) {
  return BinaryExpression<LHS, RHS, OPMin>(lhs, rhs);
}

template <class ARG, class OP>
class UnaryExpression : public Expression<UnaryExpression<ARG, OP>> {
  const double myValue;

  const ARG arg;

  const double dArg = 0.0;

public:
  explicit UnaryExpression(const Expression<ARG> &a)
      : myValue(OP::eval(a.value(), 0.0)), arg(static_cast<const ARG &>(a)) {}

  explicit UnaryExpression(const Expression<ARG> &a, const double b)
      : myValue(OP::eval(a.value(), b)), arg(static_cast<const ARG &>(a)),
        dArg(b) {}

  double value() const { return myValue; }

  enum { numNumbers = ARG::numNumbers };
  template <size_t N, size_t n>
  void pushAdjoint(Node &exprNode, const double adjoint) const {

    if (ARG::numNumbers > 0) {
      arg.template pushAdjoint<N, n>(
          exprNode, adjoint * OP::derivative(arg.value(), value(), dArg));
    }
  }
};

struct OPExp {
  static const double eval(const double r, const double d) { return exp(r); }

  static const double derivative(const double r, const double v,
                                 const double d) {
    return v;
  }
};

struct OPLog {
  static const double eval(const double r, const double d) { return log(r); }

  static const double derivative(const double r, const double v,
                                 const double d) {
    return 1.0 / r;
  }
};

struct OPSqrt {
  static const double eval(const double r, const double d) { return sqrt(r); }

  static const double derivative(const double r, const double v,
                                 const double d) {
    return 0.5 / v;
  }
};

struct OPFabs {
  static const double eval(const double r, const double d) { return fabs(r); }

  static const double derivative(const double r, const double v,
                                 const double d) {
    return r > 0.0 ? 1.0 : -1.0;
  }
};

struct OPNormalDens {
  static const double eval(const double r, const double d) {
    return normalDens(r);
  }

  static const double derivative(const double r, const double v,
                                 const double d) {
    return -r * v;
  }
};

struct OPNormalCdf {
  static const double eval(const double r, const double d) {
    return normalCdf(r);
  }

  static const double derivative(const double r, const double v,
                                 const double d) {
    return normalDens(r);
  }
};

//  Binary operators with a double on one side

//  * double or double *
struct OPMultD {
  static const double eval(const double r, const double d) { return r * d; }

  static const double derivative(const double r, const double v,
                                 const double d) {
    return d;
  }
};

struct OPAddD {
  static const double eval(const double r, const double d) { return r + d; }

  static const double derivative(const double r, const double v,
                                 const double d) {
    return 1.0;
  }
};

struct OPSubDL {
  static const double eval(const double r, const double d) { return d - r; }

  static const double derivative(const double r, const double v,
                                 const double d) {
    return -1.0;
  }
};

struct OPSubDR {
  static const double eval(const double r, const double d) { return r - d; }

  static const double derivative(const double r, const double v,
                                 const double d) {
    return 1.0;
  }
};

struct OPDivDL {
  static const double eval(const double r, const double d) { return d / r; }

  static const double derivative(const double r, const double v,
                                 const double d) {
    return -d / r / r;
  }
};

struct OPDivDR {
  static const double eval(const double r, const double d) { return r / d; }

  static const double derivative(const double r, const double v,
                                 const double d) {
    return 1.0 / d;
  }
};

struct OPPowDL {
  static const double eval(const double r, const double d) { return pow(d, r); }

  static const double derivative(const double r, const double v,
                                 const double d) {
    return log(d) * v;
  }
};

struct OPPowDR {
  static const double eval(const double r, const double d) { return pow(r, d); }

  static const double derivative(const double r, const double v,
                                 const double d) {
    return d * v / r;
  }
};

struct OPMaxD {
  static const double eval(const double r, const double d) { return max(r, d); }

  static const double derivative(const double r, const double v,
                                 const double d) {
    return r > d ? 1.0 : 0.0;
  }
};

struct OPMinD {
  static const double eval(const double r, const double d) { return min(r, d); }

  static const double derivative(const double r, const double v,
                                 const double d) {
    return r < d ? 1.0 : 0.0;
  }
};

template <class ARG>
UnaryExpression<ARG, OPExp> exp(const Expression<ARG> &arg) {
  return UnaryExpression<ARG, OPExp>(arg);
}

template <class ARG>
UnaryExpression<ARG, OPLog> log(const Expression<ARG> &arg) {
  return UnaryExpression<ARG, OPLog>(arg);
}

template <class ARG>
UnaryExpression<ARG, OPSqrt> sqrt(const Expression<ARG> &arg) {
  return UnaryExpression<ARG, OPSqrt>(arg);
}

template <class ARG>
UnaryExpression<ARG, OPFabs> fabs(const Expression<ARG> &arg) {
  return UnaryExpression<ARG, OPFabs>(arg);
}

template <class ARG>
UnaryExpression<ARG, OPNormalDens> normalDens(const Expression<ARG> &arg) {
  return UnaryExpression<ARG, OPNormalDens>(arg);
}

template <class ARG>
UnaryExpression<ARG, OPNormalCdf> normalCdf(const Expression<ARG> &arg) {
  return UnaryExpression<ARG, OPNormalCdf>(arg);
}

//  Overloading continued,
//      binary operators with a double on one side

template <class ARG>
UnaryExpression<ARG, OPMultD> operator*(const double d,
                                        const Expression<ARG> &rhs) {
  return UnaryExpression<ARG, OPMultD>(rhs, d);
}

template <class ARG>
UnaryExpression<ARG, OPMultD> operator*(const Expression<ARG> &lhs,
                                        const double d) {
  return UnaryExpression<ARG, OPMultD>(lhs, d);
}

template <class ARG>
UnaryExpression<ARG, OPAddD> operator+(const double d,
                                       const Expression<ARG> &rhs) {
  return UnaryExpression<ARG, OPAddD>(rhs, d);
}

template <class ARG>
UnaryExpression<ARG, OPAddD> operator+(const Expression<ARG> &lhs,
                                       const double d) {
  return UnaryExpression<ARG, OPAddD>(lhs, d);
}

template <class ARG>
UnaryExpression<ARG, OPSubDL> operator-(const double d,
                                        const Expression<ARG> &rhs) {
  return UnaryExpression<ARG, OPSubDL>(rhs, d);
}

template <class ARG>
UnaryExpression<ARG, OPSubDR> operator-(const Expression<ARG> &lhs,
                                        const double d) {
  return UnaryExpression<ARG, OPSubDR>(lhs, d);
}

template <class ARG>
UnaryExpression<ARG, OPDivDL> operator/(const double d,
                                        const Expression<ARG> &rhs) {
  return UnaryExpression<ARG, OPDivDL>(rhs, d);
}

template <class ARG>
UnaryExpression<ARG, OPDivDR> operator/(const Expression<ARG> &lhs,
                                        const double d) {
  return UnaryExpression<ARG, OPDivDR>(lhs, d);
}

template <class ARG>
UnaryExpression<ARG, OPPowDL> pow(const double d, const Expression<ARG> &rhs) {
  return UnaryExpression<ARG, OPPowDL>(rhs, d);
}

template <class ARG>
UnaryExpression<ARG, OPPowDR> pow(const Expression<ARG> &lhs, const double d) {
  return UnaryExpression<ARG, OPPowDR>(lhs, d);
}

template <class ARG>
UnaryExpression<ARG, OPMaxD> max(const double d, const Expression<ARG> &rhs) {
  return UnaryExpression<ARG, OPMaxD>(rhs, d);
}

template <class ARG>
UnaryExpression<ARG, OPMaxD> max(const Expression<ARG> &lhs, const double d) {
  return UnaryExpression<ARG, OPMaxD>(lhs, d);
}

template <class ARG>
UnaryExpression<ARG, OPMinD> min(const double d, const Expression<ARG> &rhs) {
  return UnaryExpression<ARG, OPMinD>(rhs, d);
}

template <class ARG>
UnaryExpression<ARG, OPMinD> min(const Expression<ARG> &lhs, const double d) {
  return UnaryExpression<ARG, OPMinD>(lhs, d);
}

template <class E, class F>
bool operator==(const Expression<E> &lhs, const Expression<F> &rhs) {
  return lhs.value() == rhs.value();
}
template <class E>
bool operator==(const Expression<E> &lhs, const double &rhs) {
  return lhs.value() == rhs;
}
template <class E>
bool operator==(const double &lhs, const Expression<E> &rhs) {
  return lhs == rhs.value();
}

template <class E, class F>
bool operator!=(const Expression<E> &lhs, const Expression<F> &rhs) {
  return lhs.value() != rhs.value();
}
template <class E>
bool operator!=(const Expression<E> &lhs, const double &rhs) {
  return lhs.value() != rhs;
}
template <class E>
bool operator!=(const double &lhs, const Expression<E> &rhs) {
  return lhs != rhs.value();
}

template <class E, class F>
bool operator<(const Expression<E> &lhs, const Expression<F> &rhs) {
  return lhs.value() < rhs.value();
}
template <class E> bool operator<(const Expression<E> &lhs, const double &rhs) {
  return lhs.value() < rhs;
}
template <class E> bool operator<(const double &lhs, const Expression<E> &rhs) {
  return lhs < rhs.value();
}

template <class E, class F>
bool operator>(const Expression<E> &lhs, const Expression<F> &rhs) {
  return lhs.value() > rhs.value();
}
template <class E> bool operator>(const Expression<E> &lhs, const double &rhs) {
  return lhs.value() > rhs;
}
template <class E> bool operator>(const double &lhs, const Expression<E> &rhs) {
  return lhs > rhs.value();
}

template <class E, class F>
bool operator<=(const Expression<E> &lhs, const Expression<F> &rhs) {
  return lhs.value() <= rhs.value();
}
template <class E>
bool operator<=(const Expression<E> &lhs, const double &rhs) {
  return lhs.value() <= rhs;
}
template <class E>
bool operator<=(const double &lhs, const Expression<E> &rhs) {
  return lhs <= rhs.value();
}

template <class E, class F>
bool operator>=(const Expression<E> &lhs, const Expression<F> &rhs) {
  return lhs.value() >= rhs.value();
}
template <class E>
bool operator>=(const Expression<E> &lhs, const double &rhs) {
  return lhs.value() >= rhs;
}
template <class E>
bool operator>=(const double &lhs, const Expression<E> &rhs) {
  return lhs >= rhs.value();
}

template <class RHS>
UnaryExpression<RHS, OPSubDL> operator-(const Expression<RHS> &rhs) {
  return 0.0 - rhs;
}

template <class RHS> Expression<RHS> operator+(const Expression<RHS> &rhs) {
  return rhs;
}

class Number : public Expression<Number> {

  double myValue;
  Node *myNode;

  template <size_t N> Node *createMultiNode() { return tape->recordNode<N>(); }

  template <class E> void fromExpr(const Expression<E> &e) {

    auto *node = createMultiNode<E::numNumbers>();

    static_cast<const E &>(e).template pushAdjoint<E::numNumbers, 0>(*node,
                                                                     1.0);
    myNode = node;
  }

public:
  enum { numNumbers = 1 };

  template <size_t N, size_t n>
  void pushAdjoint(Node &exprNode, const double adjoint) const {
    exprNode.pAdjPtrs[n] = Tape::multi ? myNode->pAdjoints : &myNode->mAdjoint;
    exprNode.pDerivatives[n] = adjoint;
  }

  static thread_local Tape *tape;

  Number() {}

  explicit Number(const double val) : myValue(val) {
    myNode = createMultiNode<0>();
  }

  Number &operator=(const double val) {
    myValue = val;
    myNode = createMultiNode<0>();
    return *this;
  }

  template <class E> Number(const Expression<E> &e) : myValue(e.value()) {
    fromExpr<E>(static_cast<const E &>(e));
  }

  template <class E> Number &operator=(const Expression<E> &e) {
    myValue = e.value();
    fromExpr<E>(static_cast<const E &>(e));
    return *this;
  }

  explicit operator double &() { return myValue; }
  explicit operator double() const { return myValue; }

  void putOnTape() { myNode = createMultiNode<0>(); }

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
    auto it = tape->find(myNode);
    while (it != propagateTo) {
      it->propagateOne();
      --it;
    }
    it->propagateOne();
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

  template <class E> Number &operator+=(const Expression<E> &e) {
    *this = *this + e;
    return *this;
  }

  template <class E> Number &operator*=(const Expression<E> &e) {
    *this = *this * e;
    return *this;
  }

  template <class E> Number &operator-=(const Expression<E> &e) {
    *this = *this - e;
    return *this;
  }

  template <class E> Number &operator/=(const Expression<E> &e) {
    *this = *this / e;
    return *this;
  }

  Number &operator+=(const double &e) {
    *this = *this + e;
    return *this;
  }

  Number &operator*=(const double &e) {
    *this = *this * e;
    return *this;
  }

  Number &operator-=(const double &e) {
    *this = *this - e;
    return *this;
  }

  Number &operator/=(const double &e) {
    *this = *this / e;
    return *this;
  }
};

#endif // AADEXPR_HPP