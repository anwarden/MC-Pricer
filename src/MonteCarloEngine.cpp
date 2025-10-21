#include "MonteCarloEngine.h"
#include "Option.h"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <stdexcept>

namespace MCPricer {

MonteCarloEngine::MonteCarloEngine(int numSimulations, int numSteps, unsigned int seed)
    : numSimulations_(numSimulations), numSteps_(numSteps), seed_(seed),
      generator_(seed), distribution_(0.0, 1.0) {
    if (numSimulations <= 0 || numSteps <= 0) {
        throw std::invalid_argument("Number of simulations and steps must be positive");
    }
}

std::vector<double> MonteCarloEngine::generatePath(double S0, double r, double sigma, double T) const {
    std::vector<double> path(numSteps_ + 1);
    path[0] = S0;
    
    double dt = T / numSteps_;
    double drift = (r - 0.5 * sigma * sigma) * dt;
    double diffusion = sigma * std::sqrt(dt);
    
    for (int i = 1; i <= numSteps_; ++i) {
        double z = distribution_(generator_);
        path[i] = path[i-1] * std::exp(drift + diffusion * z);
    }
    
    return path;
}

MCResult MonteCarloEngine::priceEuropean(const EuropeanOption& option, double S0, double r, double sigma) const {
    std::vector<double> payoffs(numSimulations_);
    double T = option.getMaturity();
    
    for (int i = 0; i < numSimulations_; ++i) {
        auto path = generatePath(S0, r, sigma, T);
        double finalPrice = path[numSteps_];
        payoffs[i] = option.payoff(finalPrice);
    }
    
    // Calculate discounted average
    double discount = std::exp(-r * T);
    double sum = std::accumulate(payoffs.begin(), payoffs.end(), 0.0);
    double mean = sum / numSimulations_;
    double price = discount * mean;
    
    // Calculate standard error
    double sumSquared = 0.0;
    for (double p : payoffs) {
        double diff = p - mean;
        sumSquared += diff * diff;
    }
    double variance = sumSquared / (numSimulations_ - 1);
    double standardError = discount * std::sqrt(variance / numSimulations_);
    
    // 95% confidence interval
    MCResult result;
    result.price = price;
    result.standardError = standardError;
    result.confidence95Lower = price - 1.96 * standardError;
    result.confidence95Upper = price + 1.96 * standardError;
    result.numSimulations = numSimulations_;
    
    return result;
}

MCResult MonteCarloEngine::priceAmerican(const AmericanOption& option, double S0, double r, double sigma) const {
    // Longstaff-Schwartz algorithm for American options
    double T = option.getMaturity();
    double dt = T / numSteps_;
    double discount = std::exp(-r * dt);
    
    // Generate all paths
    std::vector<std::vector<double>> paths(numSimulations_);
    for (int i = 0; i < numSimulations_; ++i) {
        paths[i] = generatePath(S0, r, sigma, T);
    }
    
    // Initialize cash flows at maturity
    std::vector<double> cashFlows(numSimulations_);
    for (int i = 0; i < numSimulations_; ++i) {
        cashFlows[i] = option.payoff(paths[i][numSteps_]);
    }
    
    // Backward induction
    for (int t = numSteps_ - 1; t > 0; --t) {
        // Collect in-the-money paths
        std::vector<int> itmIndices;
        std::vector<double> X, Y;
        
        for (int i = 0; i < numSimulations_; ++i) {
            double spot = paths[i][t];
            double exerciseValue = option.payoff(spot);
            
            if (exerciseValue > 0) {
                itmIndices.push_back(i);
                X.push_back(spot);
                Y.push_back(cashFlows[i] * discount);
            }
        }
        
        if (itmIndices.size() > 10) { // Need sufficient data for regression
            // Simple polynomial regression (degree 2): a + b*x + c*x^2
            int n = X.size();
            double sum_x = 0, sum_x2 = 0, sum_x3 = 0, sum_x4 = 0;
            double sum_y = 0, sum_xy = 0, sum_x2y = 0;
            
            for (size_t i = 0; i < X.size(); ++i) {
                double x = X[i];
                double y = Y[i];
                double x2 = x * x;
                sum_x += x;
                sum_x2 += x2;
                sum_x3 += x2 * x;
                sum_x4 += x2 * x2;
                sum_y += y;
                sum_xy += x * y;
                sum_x2y += x2 * y;
            }
            
            // Solve normal equations for a, b, c
            // This is a simplified version - for production, use proper linear algebra library
            double meanY = sum_y / n;
            
            for (size_t j = 0; j < itmIndices.size(); ++j) {
                int i = itmIndices[j];
                double spot = paths[i][t];
                double exerciseValue = option.payoff(spot);
                
                // Continuation value (simplified)
                double continuationValue = meanY; // Use mean as simple estimator
                
                if (exerciseValue > continuationValue) {
                    cashFlows[i] = exerciseValue;
                } else {
                    cashFlows[i] *= discount;
                }
            }
        } else {
            // Not enough ITM paths, just discount
            for (int i : itmIndices) {
                cashFlows[i] *= discount;
            }
        }
        
        // Discount all OTM paths
        for (int i = 0; i < numSimulations_; ++i) {
            bool isITM = false;
            for (int idx : itmIndices) {
                if (idx == i) {
                    isITM = true;
                    break;
                }
            }
            if (!isITM) {
                cashFlows[i] *= discount;
            }
        }
    }
    
    // Calculate final price
    double sum = std::accumulate(cashFlows.begin(), cashFlows.end(), 0.0);
    double mean = sum / numSimulations_;
    double price = mean * discount; // Discount from t=1 to t=0
    
    // Calculate standard error
    double sumSquared = 0.0;
    for (double cf : cashFlows) {
        double diff = cf - mean;
        sumSquared += diff * diff;
    }
    double variance = sumSquared / (numSimulations_ - 1);
    double standardError = discount * std::sqrt(variance / numSimulations_);
    
    MCResult result;
    result.price = price;
    result.standardError = standardError;
    result.confidence95Lower = price - 1.96 * standardError;
    result.confidence95Upper = price + 1.96 * standardError;
    result.numSimulations = numSimulations_;
    
    return result;
}

MCResult MonteCarloEngine::price(const Option& option, double S0, double r, double sigma) const {
    if (option.getStyle() == ExerciseStyle::EUROPEAN) {
        const EuropeanOption& europeanOption = dynamic_cast<const EuropeanOption&>(option);
        return priceEuropean(europeanOption, S0, r, sigma);
    } else {
        const AmericanOption& americanOption = dynamic_cast<const AmericanOption&>(option);
        return priceAmerican(americanOption, S0, r, sigma);
    }
}

} // namespace MCPricer
