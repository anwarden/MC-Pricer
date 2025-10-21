#include "BlackScholes.h"
#include <cmath>
#include <stdexcept>
#include <algorithm>

namespace MCPricer {

BlackScholes::BlackScholes(double S, double K, double r, double T, double sigma)
    : S_(S), K_(K), r_(r), T_(T), sigma_(sigma) {
    if (S <= 0 || K <= 0 || T <= 0 || sigma <= 0) {
        throw std::invalid_argument("All parameters must be positive");
    }
}

double BlackScholes::normalCDF(double x) const {
    return 0.5 * std::erfc(-x * M_SQRT1_2);
}

double BlackScholes::normalPDF(double x) const {
    return std::exp(-0.5 * x * x) / std::sqrt(2.0 * M_PI);
}

double BlackScholes::d1() const {
    return (std::log(S_ / K_) + (r_ + 0.5 * sigma_ * sigma_) * T_) / (sigma_ * std::sqrt(T_));
}

double BlackScholes::d2() const {
    return d1() - sigma_ * std::sqrt(T_);
}

double BlackScholes::callPrice() const {
    double d1_val = d1();
    double d2_val = d2();
    return S_ * normalCDF(d1_val) - K_ * std::exp(-r_ * T_) * normalCDF(d2_val);
}

double BlackScholes::putPrice() const {
    double d1_val = d1();
    double d2_val = d2();
    return K_ * std::exp(-r_ * T_) * normalCDF(-d2_val) - S_ * normalCDF(-d1_val);
}

double BlackScholes::price(OptionType type) const {
    return (type == OptionType::CALL) ? callPrice() : putPrice();
}

double BlackScholes::delta(OptionType type) const {
    double d1_val = d1();
    if (type == OptionType::CALL) {
        return normalCDF(d1_val);
    } else {
        return normalCDF(d1_val) - 1.0;
    }
}

double BlackScholes::gamma() const {
    double d1_val = d1();
    return normalPDF(d1_val) / (S_ * sigma_ * std::sqrt(T_));
}

double BlackScholes::vega() const {
    double d1_val = d1();
    return S_ * normalPDF(d1_val) * std::sqrt(T_);
}

double BlackScholes::theta(OptionType type) const {
    double d1_val = d1();
    double d2_val = d2();
    double term1 = -(S_ * normalPDF(d1_val) * sigma_) / (2.0 * std::sqrt(T_));
    
    if (type == OptionType::CALL) {
        double term2 = r_ * K_ * std::exp(-r_ * T_) * normalCDF(d2_val);
        return term1 - term2;
    } else {
        double term2 = r_ * K_ * std::exp(-r_ * T_) * normalCDF(-d2_val);
        return term1 + term2;
    }
}

double BlackScholes::rho(OptionType type) const {
    double d2_val = d2();
    if (type == OptionType::CALL) {
        return K_ * T_ * std::exp(-r_ * T_) * normalCDF(d2_val);
    } else {
        return -K_ * T_ * std::exp(-r_ * T_) * normalCDF(-d2_val);
    }
}

double BlackScholes::impliedVolatility(double marketPrice, double S, double K, 
                                      double r, double T, OptionType type,
                                      double tolerance, int maxIterations) {
    // Newton-Raphson method
    double sigma = 0.2; // Initial guess
    
    for (int i = 0; i < maxIterations; ++i) {
        BlackScholes bs(S, K, r, T, sigma);
        double price = bs.price(type);
        double diff = price - marketPrice;
        
        if (std::abs(diff) < tolerance) {
            return sigma;
        }
        
        double vega = bs.vega();
        if (vega < 1e-10) {
            throw std::runtime_error("Vega too small, cannot compute implied volatility");
        }
        
        sigma = sigma - diff / vega;
        
        // Keep sigma in reasonable bounds
        sigma = std::max(0.001, std::min(sigma, 5.0));
    }
    
    throw std::runtime_error("Implied volatility did not converge");
}

} // namespace MCPricer
