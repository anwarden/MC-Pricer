#include "MonteCarloEngine.h"
#include <cmath>
#include <algorithm>
#include <numeric>

MonteCarloEngine::MonteCarloEngine(unsigned int simulations, unsigned int steps, unsigned long seed)
    : numSimulations(simulations), numSteps(steps), seed(seed), 
      rng(seed), normalDist(0.0, 1.0) {}

std::vector<double> MonteCarloEngine::simulatePaths(const Option& option) const {
    std::vector<double> finalPrices(numSimulations);
    
    double S0 = option.getSpotPrice();
    double r = option.getRiskFreeRate();
    double sigma = option.getVolatility();
    double T = option.getTimeToMaturity();
    double dt = T / numSteps;
    
    // For thread-safety in const method, create local RNG
    std::mt19937 localRng(seed);
    std::normal_distribution<double> localNormalDist(0.0, 1.0);
    
    for (unsigned int i = 0; i < numSimulations; ++i) {
        double S = S0;
        
        // Simulate price path using geometric Brownian motion
        for (unsigned int j = 0; j < numSteps; ++j) {
            double z = localNormalDist(localRng);
            S *= std::exp((r - 0.5 * sigma * sigma) * dt + sigma * std::sqrt(dt) * z);
        }
        
        finalPrices[i] = S;
    }
    
    return finalPrices;
}

std::pair<double, double> MonteCarloEngine::priceEuropeanOption(const Option& option) const {
    std::vector<double> finalPrices = simulatePaths(option);
    
    double K = option.getStrikePrice();
    double r = option.getRiskFreeRate();
    double T = option.getTimeToMaturity();
    std::string type = option.getOptionType();
    
    // Calculate payoffs
    std::vector<double> payoffs(numSimulations);
    for (unsigned int i = 0; i < numSimulations; ++i) {
        if (type == "call") {
            payoffs[i] = std::max(finalPrices[i] - K, 0.0);
        } else {
            payoffs[i] = std::max(K - finalPrices[i], 0.0);
        }
    }
    
    // Calculate mean and standard error
    double sumPayoffs = std::accumulate(payoffs.begin(), payoffs.end(), 0.0);
    double meanPayoff = sumPayoffs / numSimulations;
    
    // Discount to present value
    double price = meanPayoff * std::exp(-r * T);
    
    // Calculate standard error
    double sumSquaredDiff = 0.0;
    for (double payoff : payoffs) {
        double diff = payoff - meanPayoff;
        sumSquaredDiff += diff * diff;
    }
    double variance = sumSquaredDiff / (numSimulations - 1);
    double standardError = std::sqrt(variance / numSimulations) * std::exp(-r * T);
    
    return std::make_pair(price, standardError);
}
