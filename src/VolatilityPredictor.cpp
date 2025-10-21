#include "VolatilityPredictor.h"
#include <cmath>
#include <numeric>
#include <algorithm>
#include <stdexcept>

namespace MCPricer {

// Historical Volatility Implementation
HistoricalVolatility::HistoricalVolatility(int windowSize)
    : windowSize_(windowSize) {
    if (windowSize <= 1) {
        throw std::invalid_argument("Window size must be greater than 1");
    }
}

double HistoricalVolatility::predict(const std::vector<double>& returns) {
    if (returns.empty()) {
        throw std::invalid_argument("Returns vector is empty");
    }
    
    // Use last windowSize_ returns or all if fewer
    int n = std::min(windowSize_, static_cast<int>(returns.size()));
    
    // Calculate mean
    double sum = 0.0;
    for (int i = returns.size() - n; i < static_cast<int>(returns.size()); ++i) {
        sum += returns[i];
    }
    double mean = sum / n;
    
    // Calculate variance
    double variance = 0.0;
    for (int i = returns.size() - n; i < static_cast<int>(returns.size()); ++i) {
        double diff = returns[i] - mean;
        variance += diff * diff;
    }
    variance /= (n - 1);
    
    // Annualize (assuming daily returns)
    return std::sqrt(variance * 252.0);
}

// EWMA Volatility Implementation
EWMAVolatility::EWMAVolatility(double lambda)
    : lambda_(lambda) {
    if (lambda <= 0 || lambda >= 1) {
        throw std::invalid_argument("Lambda must be between 0 and 1");
    }
}

double EWMAVolatility::predict(const std::vector<double>& returns) {
    if (returns.empty()) {
        throw std::invalid_argument("Returns vector is empty");
    }
    
    // Initialize with first squared return
    double variance = returns[0] * returns[0];
    
    // EWMA recursion
    for (size_t i = 1; i < returns.size(); ++i) {
        variance = lambda_ * variance + (1.0 - lambda_) * returns[i] * returns[i];
    }
    
    // Annualize (assuming daily returns)
    return std::sqrt(variance * 252.0);
}

// ML Volatility Predictor Implementation
MLVolatilityPredictor::MLVolatilityPredictor(const std::string& modulePath)
    : pythonModulePath_(modulePath), isPythonInitialized_(false) {
    // In a full implementation, this would initialize Python interpreter
    // and load the ML model. For now, we'll provide a placeholder.
}

double MLVolatilityPredictor::predict(const std::vector<double>& returns) {
    if (returns.empty()) {
        throw std::invalid_argument("Returns vector is empty");
    }
    
    // Placeholder: In production, this would call Python ML model
    // For now, fall back to EWMA as a sophisticated estimator
    EWMAVolatility ewma(0.94);
    return ewma.predict(returns);
}

} // namespace MCPricer
