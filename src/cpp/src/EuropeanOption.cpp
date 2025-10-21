#include "EuropeanOption.h"
#include <cmath>
#include <algorithm>

EuropeanOption::EuropeanOption(double S, double K, double r, double sigma, double T, const std::string& type)
    : Option(S, K, r, sigma, T, type) {}

double EuropeanOption::d1() const {
    if (timeToMaturity == 0) return 0.0;
    return (std::log(spotPrice / strikePrice) + 
            (riskFreeRate + 0.5 * volatility * volatility) * timeToMaturity) /
           (volatility * std::sqrt(timeToMaturity));
}

double EuropeanOption::d2() const {
    if (timeToMaturity == 0) return 0.0;
    return d1() - volatility * std::sqrt(timeToMaturity);
}

double EuropeanOption::normalCDF(double x) const {
    // Approximation of cumulative normal distribution
    return 0.5 * std::erfc(-x / std::sqrt(2.0));
}

double EuropeanOption::normalPDF(double x) const {
    // Probability density function of standard normal distribution
    return std::exp(-0.5 * x * x) / std::sqrt(2.0 * M_PI);
}

double EuropeanOption::calculatePrice() const {
    if (timeToMaturity == 0) {
        // At expiration, option value is intrinsic value
        if (optionType == "call") {
            return std::max(spotPrice - strikePrice, 0.0);
        } else {
            return std::max(strikePrice - spotPrice, 0.0);
        }
    }

    double d1_val = d1();
    double d2_val = d2();
    double discountFactor = std::exp(-riskFreeRate * timeToMaturity);

    if (optionType == "call") {
        return spotPrice * normalCDF(d1_val) - 
               strikePrice * discountFactor * normalCDF(d2_val);
    } else {
        return strikePrice * discountFactor * normalCDF(-d2_val) - 
               spotPrice * normalCDF(-d1_val);
    }
}

double EuropeanOption::calculateDelta() const {
    if (timeToMaturity == 0) {
        if (optionType == "call") {
            return (spotPrice > strikePrice) ? 1.0 : 0.0;
        } else {
            return (spotPrice < strikePrice) ? -1.0 : 0.0;
        }
    }

    double d1_val = d1();
    if (optionType == "call") {
        return normalCDF(d1_val);
    } else {
        return normalCDF(d1_val) - 1.0;
    }
}

double EuropeanOption::calculateGamma() const {
    if (timeToMaturity == 0) return 0.0;
    
    double d1_val = d1();
    return normalPDF(d1_val) / (spotPrice * volatility * std::sqrt(timeToMaturity));
}

double EuropeanOption::calculateVega() const {
    if (timeToMaturity == 0) return 0.0;
    
    double d1_val = d1();
    return spotPrice * normalPDF(d1_val) * std::sqrt(timeToMaturity) / 100.0;
}

double EuropeanOption::calculateTheta() const {
    if (timeToMaturity == 0) return 0.0;
    
    double d1_val = d1();
    double d2_val = d2();
    double sqrtT = std::sqrt(timeToMaturity);
    
    double term1 = -(spotPrice * normalPDF(d1_val) * volatility) / (2.0 * sqrtT);
    
    if (optionType == "call") {
        double term2 = riskFreeRate * strikePrice * 
                      std::exp(-riskFreeRate * timeToMaturity) * normalCDF(d2_val);
        return (term1 - term2) / 365.0;
    } else {
        double term2 = riskFreeRate * strikePrice * 
                      std::exp(-riskFreeRate * timeToMaturity) * normalCDF(-d2_val);
        return (term1 + term2) / 365.0;
    }
}

double EuropeanOption::calculateRho() const {
    if (timeToMaturity == 0) return 0.0;
    
    double d2_val = d2();
    double discountFactor = std::exp(-riskFreeRate * timeToMaturity);
    
    if (optionType == "call") {
        return strikePrice * timeToMaturity * discountFactor * normalCDF(d2_val) / 100.0;
    } else {
        return -strikePrice * timeToMaturity * discountFactor * normalCDF(-d2_val) / 100.0;
    }
}
