#include "EuropeanOption.h"
#include "MonteCarloEngine.h"
#include "VolatilityInterface.h"
#include <iostream>
#include <iomanip>
#include <vector>

void printGreeks(const EuropeanOption& option) {
    std::cout << "\n=== Greeks ===" << std::endl;
    std::cout << "Delta:   " << std::fixed << std::setprecision(4) << option.calculateDelta() << std::endl;
    std::cout << "Gamma:   " << std::fixed << std::setprecision(4) << option.calculateGamma() << std::endl;
    std::cout << "Vega:    " << std::fixed << std::setprecision(4) << option.calculateVega() << std::endl;
    std::cout << "Theta:   " << std::fixed << std::setprecision(4) << option.calculateTheta() << std::endl;
    std::cout << "Rho:     " << std::fixed << std::setprecision(4) << option.calculateRho() << std::endl;
}

int main() {
    std::cout << "=== MC-Pricer: Production-Grade Option Pricing Engine ===" << std::endl;
    std::cout << "Combining Black-Scholes Analytics with Monte Carlo Simulation\n" << std::endl;

    // Example parameters
    double S = 100.0;    // Spot price
    double K = 105.0;    // Strike price
    double r = 0.05;     // Risk-free rate (5%)
    double T = 1.0;      // Time to maturity (1 year)
    std::string type = "call";

    // Example: Use historical volatility
    std::vector<double> historicalPrices = {95.0, 97.0, 99.0, 101.0, 98.0, 
                                            100.0, 102.0, 104.0, 103.0, 100.0};
    
    HistoricalVolatility histVol(10, 252);
    double sigma = histVol.predictVolatility(historicalPrices);
    
    std::cout << "=== Input Parameters ===" << std::endl;
    std::cout << "Spot Price (S):        $" << std::fixed << std::setprecision(2) << S << std::endl;
    std::cout << "Strike Price (K):      $" << std::fixed << std::setprecision(2) << K << std::endl;
    std::cout << "Risk-free Rate (r):    " << std::fixed << std::setprecision(2) << (r * 100) << "%" << std::endl;
    std::cout << "Time to Maturity (T):  " << std::fixed << std::setprecision(2) << T << " years" << std::endl;
    std::cout << "Volatility (σ):        " << std::fixed << std::setprecision(2) << (sigma * 100) << "% (historical)" << std::endl;
    std::cout << "Option Type:           " << type << std::endl;

    // Create option
    EuropeanOption option(S, K, r, sigma, T, type);

    // Black-Scholes analytical pricing
    std::cout << "\n=== Black-Scholes Analytical Pricing ===" << std::endl;
    double bsPrice = option.calculatePrice();
    std::cout << "Option Price: $" << std::fixed << std::setprecision(4) << bsPrice << std::endl;
    printGreeks(option);

    // Monte Carlo simulation
    std::cout << "\n=== Monte Carlo Simulation ===" << std::endl;
    MonteCarloEngine mcEngine(100000, 252, 42);
    std::cout << "Simulations: " << mcEngine.getNumSimulations() << std::endl;
    std::cout << "Time Steps:  " << mcEngine.getNumSteps() << std::endl;
    
    auto [mcPrice, mcError] = mcEngine.priceEuropeanOption(option);
    std::cout << "Option Price: $" << std::fixed << std::setprecision(4) << mcPrice 
              << " ± $" << std::fixed << std::setprecision(4) << mcError << std::endl;
    
    double difference = std::abs(bsPrice - mcPrice);
    double percentDiff = (difference / bsPrice) * 100.0;
    std::cout << "\n=== Comparison ===" << std::endl;
    std::cout << "Absolute Difference: $" << std::fixed << std::setprecision(4) << difference << std::endl;
    std::cout << "Relative Difference: " << std::fixed << std::setprecision(2) << percentDiff << "%" << std::endl;

    // Example with different option type
    std::cout << "\n\n=== Put Option Example ===" << std::endl;
    EuropeanOption putOption(S, K, r, sigma, T, "put");
    std::cout << "Black-Scholes Price: $" << std::fixed << std::setprecision(4) 
              << putOption.calculatePrice() << std::endl;
    
    auto [putMCPrice, putMCError] = mcEngine.priceEuropeanOption(putOption);
    std::cout << "Monte Carlo Price:   $" << std::fixed << std::setprecision(4) 
              << putMCPrice << " ± $" << std::fixed << std::setprecision(4) << putMCError << std::endl;

    std::cout << "\n=== Pricing Complete ===" << std::endl;

    return 0;
}
