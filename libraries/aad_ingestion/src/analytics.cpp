#include "analytics.hpp"

double blackScholesIvol(const double spot, const double strike,
                        const double prem, const double mat) {
  if (prem <= max(0.0, spot - strike) + EPS)
    return 0.0;

  double p, pu, pl;
  double u = 0.5;
  while (blackScholes(spot, strike, u, mat) < prem)
    u *= 2;
  double l = 0.05;
  while (blackScholes(spot, strike, l, mat) > prem)
    l /= 2;
  pu = blackScholes(spot, strike, u, mat);
  blackScholes(spot, strike, l, mat);

  while (u - l > 1.e-12) {
    const double m = 0.5 * (u + l);
    p = blackScholes(spot, strike, m, mat);
    if (p > prem) {
      u = m;
      pu = p;
    } else {
      l = m;
      pl = p;
    }
  }

  return l + (prem - pl) / (pu - pl) * (u - l);
}

double BlackScholesKO(const double spot, const double rate, const double div,
                      const double strike, const double barrier,
                      const double mat, const double vol) {
  const double std = vol * sqrt(mat);
  const double fwdFact = exp((rate - div) * mat);
  const double fwd = spot * fwdFact;
  const double disc = exp(-rate * mat);
  const double v = rate - div - 0.5 * vol * vol;
  const double d2 = (log(spot / barrier) + v * mat) / std;
  const double d2prime = (log(barrier / spot) + v * mat) / std;

  const double bar =
      blackScholes(fwd, strike, vol, mat) -
      blackScholes(fwd, barrier, vol, mat) -
      (barrier - strike) * normalCdf(d2) -
      pow(barrier / spot, 2 * v / vol / vol) *
          (blackScholes(fwdFact * barrier * barrier / spot, strike, vol, mat) -
           blackScholes(fwdFact * barrier * barrier / spot, barrier, vol, mat) -
           (barrier - strike) * normalCdf(d2prime));

  return disc * bar;
}