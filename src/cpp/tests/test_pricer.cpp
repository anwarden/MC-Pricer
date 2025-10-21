#include "EuropeanOption.h"
#include "MonteCarloEngine.h"
#include "VolatilityInterface.h"
#include <iostream>
#include <cassert>
#include <cmath>

void testEuropeanCallOption() {
    std::cout << "Testing European Call Option..." << std::endl;
    
    // Test parameters
    double S = 100.0;
    double K = 100.0;
    double r = 0.05;
    double sigma = 0.2;
    double T = 1.0;
    
    EuropeanOption option(S, K, r, sigma, T, "call");
    
    // Calculate price
    double price = option.calculatePrice();
    
    // At-the-money call option price should be positive
    assert(price > 0);
    std::cout << "  ✓ Call option price: $" << price << std::endl;
    
    // Delta should be around 0.5 for ATM option
    double delta = option.calculateDelta();
    assert(delta > 0.3 && delta < 0.7);
    std::cout << "  ✓ Delta: " << delta << std::endl;
    
    // Gamma should be positive
    double gamma = option.calculateGamma();
    assert(gamma > 0);
    std::cout << "  ✓ Gamma: " << gamma << std::endl;
    
    std::cout << "  ✓ Test passed!\n" << std::endl;
}

void testEuropeanPutOption() {
    std::cout << "Testing European Put Option..." << std::endl;
    
    double S = 100.0;
    double K = 100.0;
    double r = 0.05;
    double sigma = 0.2;
    double T = 1.0;
    
    EuropeanOption option(S, K, r, sigma, T, "put");
    
    double price = option.calculatePrice();
    assert(price > 0);
    std::cout << "  ✓ Put option price: $" << price << std::endl;
    
    // Put delta should be negative
    double delta = option.calculateDelta();
    assert(delta < 0);
    std::cout << "  ✓ Delta: " << delta << std::endl;
    
    std::cout << "  ✓ Test passed!\n" << std::endl;
}

void testPutCallParity() {
    std::cout << "Testing Put-Call Parity..." << std::endl;
    
    double S = 100.0;
    double K = 100.0;
    double r = 0.05;
    double sigma = 0.2;
    double T = 1.0;
    
    EuropeanOption call(S, K, r, sigma, T, "call");
    EuropeanOption put(S, K, r, sigma, T, "put");
    
    double callPrice = call.calculatePrice();
    double putPrice = put.calculatePrice();
    
    // Put-Call Parity: C - P = S - K*e^(-rT)
    double lhs = callPrice - putPrice;
    double rhs = S - K * std::exp(-r * T);
    double diff = std::abs(lhs - rhs);
    
    assert(diff < 1e-6);  // Should be equal within numerical precision
    std::cout << "  ✓ C - P = " << lhs << std::endl;
    std::cout << "  ✓ S - K*e^(-rT) = " << rhs << std::endl;
    std::cout << "  ✓ Difference: " << diff << std::endl;
    std::cout << "  ✓ Test passed!\n" << std::endl;
}

void testMonteCarloConvergence() {
    std::cout << "Testing Monte Carlo Convergence..." << std::endl;
    
    double S = 100.0;
    double K = 100.0;
    double r = 0.05;
    double sigma = 0.2;
    double T = 1.0;
    
    EuropeanOption option(S, K, r, sigma, T, "call");
    double bsPrice = option.calculatePrice();
    
    MonteCarloEngine mc(50000, 100, 42);
    auto [mcPrice, mcError] = mc.priceEuropeanOption(option);
    
    // MC price should be close to BS price
    double diff = std::abs(mcPrice - bsPrice);
    double percentDiff = (diff / bsPrice) * 100.0;
    
    std::cout << "  ✓ Black-Scholes: $" << bsPrice << std::endl;
    std::cout << "  ✓ Monte Carlo: $" << mcPrice << " ± $" << mcError << std::endl;
    std::cout << "  ✓ Difference: " << percentDiff << "%" << std::endl;
    
    // Should be within a few percent
    assert(percentDiff < 5.0);
    std::cout << "  ✓ Test passed!\n" << std::endl;
}

void testHistoricalVolatility() {
    std::cout << "Testing Historical Volatility..." << std::endl;
    
    std::vector<double> prices = {100.0, 102.0, 101.0, 103.0, 105.0, 
                                   104.0, 106.0, 108.0, 107.0, 110.0};
    
    HistoricalVolatility histVol(10, 252);
    double vol = histVol.predictVolatility(prices);
    
    assert(vol > 0);
    std::cout << "  ✓ Historical volatility: " << vol * 100 << "%" << std::endl;
    std::cout << "  ✓ Test passed!\n" << std::endl;
}

int main() {
    std::cout << "======================================================================" << std::endl;
    std::cout << "Running C++ Unit Tests" << std::endl;
    std::cout << "======================================================================\n" << std::endl;
    
    int passed = 0;
    int failed = 0;
    
    try {
        testEuropeanCallOption();
        passed++;
    } catch (const std::exception& e) {
        std::cerr << "  ✗ Test failed: " << e.what() << std::endl;
        failed++;
    }
    
    try {
        testEuropeanPutOption();
        passed++;
    } catch (const std::exception& e) {
        std::cerr << "  ✗ Test failed: " << e.what() << std::endl;
        failed++;
    }
    
    try {
        testPutCallParity();
        passed++;
    } catch (const std::exception& e) {
        std::cerr << "  ✗ Test failed: " << e.what() << std::endl;
        failed++;
    }
    
    try {
        testMonteCarloConvergence();
        passed++;
    } catch (const std::exception& e) {
        std::cerr << "  ✗ Test failed: " << e.what() << std::endl;
        failed++;
    }
    
    try {
        testHistoricalVolatility();
        passed++;
    } catch (const std::exception& e) {
        std::cerr << "  ✗ Test failed: " << e.what() << std::endl;
        failed++;
    }
    
    std::cout << "======================================================================" << std::endl;
    std::cout << "Test Results: " << passed << " passed, " << failed << " failed" << std::endl;
    std::cout << "======================================================================" << std::endl;
    
    return (failed == 0) ? 0 : 1;
}
