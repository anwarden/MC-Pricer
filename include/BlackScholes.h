#ifndef BLACK_SCHOLES_H
#define BLACK_SCHOLES_H

#include "Option.h"
#include <cmath>

namespace MCPricer {

class BlackScholes {
private:
    double S_;  // Current stock price
    double K_;  // Strike price
    double r_;  // Risk-free rate
    double T_;  // Time to maturity
    double sigma_; // Volatility

    // Helper functions
    double d1() const;
    double d2() const;
    double normalCDF(double x) const;
    double normalPDF(double x) const;

public:
    BlackScholes(double S, double K, double r, double T, double sigma);

    // Pricing methods
    double callPrice() const;
    double putPrice() const;
    double price(OptionType type) const;

    // Greeks
    double delta(OptionType type) const;
    double gamma() const;
    double vega() const;
    double theta(OptionType type) const;
    double rho(OptionType type) const;

    // Implied volatility
    static double impliedVolatility(double marketPrice, double S, double K, 
                                   double r, double T, OptionType type,
                                   double tolerance = 1e-6, int maxIterations = 100);
};

} // namespace MCPricer

#endif // BLACK_SCHOLES_H
