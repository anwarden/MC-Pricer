#ifndef MONTE_CARLO_ENGINE_H
#define MONTE_CARLO_ENGINE_H

#include "Option.h"
#include <vector>
#include <random>
#include <memory>

namespace MCPricer {

// Result structure to hold pricing results and statistics
struct MCResult {
    double price;
    double standardError;
    double confidence95Lower;
    double confidence95Upper;
    int numSimulations;
};

class MonteCarloEngine {
private:
    int numSimulations_;
    int numSteps_;
    unsigned int seed_;
    mutable std::mt19937 generator_;
    mutable std::normal_distribution<double> distribution_;

    // Generate a path for the underlying asset
    std::vector<double> generatePath(double S0, double r, double sigma, double T) const;

    // Price European option using Monte Carlo
    MCResult priceEuropean(const EuropeanOption& option, double S0, double r, double sigma) const;

    // Price American option using Longstaff-Schwartz
    MCResult priceAmerican(const AmericanOption& option, double S0, double r, double sigma) const;

public:
    MonteCarloEngine(int numSimulations = 100000, int numSteps = 252, unsigned int seed = 42);

    // Main pricing method
    MCResult price(const Option& option, double S0, double r, double sigma) const;

    // Getters and setters
    void setNumSimulations(int n) { numSimulations_ = n; }
    void setNumSteps(int n) { numSteps_ = n; }
    int getNumSimulations() const { return numSimulations_; }
    int getNumSteps() const { return numSteps_; }
};

} // namespace MCPricer

#endif // MONTE_CARLO_ENGINE_H
