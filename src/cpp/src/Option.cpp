#include "Option.h"
#include <stdexcept>
#include <algorithm>

Option::Option(double S, double K, double r, double sigma, double T, const std::string& type)
    : spotPrice(S), strikePrice(K), riskFreeRate(r), volatility(sigma), 
      timeToMaturity(T), optionType(type) {
    
    // Validate inputs
    if (S <= 0) throw std::invalid_argument("Spot price must be positive");
    if (K <= 0) throw std::invalid_argument("Strike price must be positive");
    if (sigma < 0) throw std::invalid_argument("Volatility must be non-negative");
    if (T < 0) throw std::invalid_argument("Time to maturity must be non-negative");
    
    // Normalize option type to lowercase
    std::string normalizedType = type;
    std::transform(normalizedType.begin(), normalizedType.end(), 
                   normalizedType.begin(), ::tolower);
    
    if (normalizedType != "call" && normalizedType != "put") {
        throw std::invalid_argument("Option type must be 'call' or 'put'");
    }
    
    optionType = normalizedType;
}
