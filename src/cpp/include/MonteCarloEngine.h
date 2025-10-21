#ifndef MONTE_CARLO_ENGINE_H
#define MONTE_CARLO_ENGINE_H

#include "Option.h"
#include <vector>
#include <random>

/**
 * @brief Monte Carlo simulation engine for option pricing
 * 
 * Uses geometric Brownian motion to simulate asset price paths
 */
class MonteCarloEngine {
private:
    unsigned int numSimulations;
    unsigned int numSteps;
    unsigned long seed;
    std::mt19937 rng;
    std::normal_distribution<double> normalDist;

public:
    MonteCarloEngine(unsigned int simulations = 100000, unsigned int steps = 252, unsigned long seed = 42);

    /**
     * @brief Price a European option using Monte Carlo simulation
     * @param option The option to price
     * @return Tuple of (price, standard error)
     */
    std::pair<double, double> priceEuropeanOption(const Option& option) const;

    /**
     * @brief Simulate asset price paths
     * @return Vector of final asset prices
     */
    std::vector<double> simulatePaths(const Option& option) const;

    // Getters and setters
    void setNumSimulations(unsigned int n) { numSimulations = n; }
    void setNumSteps(unsigned int n) { numSteps = n; }
    unsigned int getNumSimulations() const { return numSimulations; }
    unsigned int getNumSteps() const { return numSteps; }
};

#endif // MONTE_CARLO_ENGINE_H
