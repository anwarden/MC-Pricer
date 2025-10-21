#include "VolatilityInterface.h"
#include <cmath>
#include <numeric>
#include <stdexcept>

HistoricalVolatility::HistoricalVolatility(int window, int annualization)
    : windowSize(window), annualizationFactor(annualization) {}

double HistoricalVolatility::predictVolatility(const std::vector<double>& historicalPrices) const {
    if (historicalPrices.size() < 2) {
        throw std::invalid_argument("Need at least 2 historical prices");
    }
    
    // Use all data or window, whichever is smaller
    size_t dataSize = std::min(static_cast<size_t>(windowSize), historicalPrices.size());
    size_t startIdx = historicalPrices.size() - dataSize;
    
    // Calculate log returns
    std::vector<double> logReturns;
    for (size_t i = startIdx + 1; i < historicalPrices.size(); ++i) {
        if (historicalPrices[i] <= 0 || historicalPrices[i-1] <= 0) {
            throw std::invalid_argument("Historical prices must be positive");
        }
        logReturns.push_back(std::log(historicalPrices[i] / historicalPrices[i-1]));
    }
    
    // Calculate mean
    double mean = std::accumulate(logReturns.begin(), logReturns.end(), 0.0) / logReturns.size();
    
    // Calculate variance
    double sumSquaredDiff = 0.0;
    for (double ret : logReturns) {
        double diff = ret - mean;
        sumSquaredDiff += diff * diff;
    }
    double variance = sumSquaredDiff / (logReturns.size() - 1);
    
    // Annualize volatility
    double volatility = std::sqrt(variance * annualizationFactor);
    
    return volatility;
}
