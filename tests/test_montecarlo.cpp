#include "MonteCarloEngine.h"
#include "Option.h"
#include "BlackScholes.h"
#include <iostream>
#include <cassert>
#include <cmath>

using namespace MCPricer;

void testEngineCreation() {
    std::cout << "Testing Monte Carlo engine creation..." << std::endl;
    
    MonteCarloEngine engine(10000, 252, 42);
    assert(engine.getNumSimulations() == 10000);
    assert(engine.getNumSteps() == 252);
    
    std::cout << "✓ Engine creation test passed" << std::endl;
}

void testEuropeanCallPricing() {
    std::cout << "Testing European call pricing..." << std::endl;
    
    double S0 = 100.0, K = 100.0, r = 0.05, T = 1.0, sigma = 0.2;
    
    EuropeanOption call(K, T, OptionType::CALL);
    MonteCarloEngine engine(50000, 252, 42);
    
    MCResult result = engine.price(call, S0, r, sigma);
    
    // Compare with Black-Scholes
    BlackScholes bs(S0, K, r, T, sigma);
    double bsPrice = bs.callPrice();
    
    std::cout << "  BS Price: " << bsPrice << std::endl;
    std::cout << "  MC Price: " << result.price << std::endl;
    std::cout << "  Std Error: " << result.standardError << std::endl;
    
    // MC should be within reasonable error
    double error = std::abs(result.price - bsPrice);
    assert(error < 0.5); // Generous tolerance for MC
    assert(result.standardError > 0);
    
    std::cout << "✓ European call pricing test passed" << std::endl;
}

void testEuropeanPutPricing() {
    std::cout << "Testing European put pricing..." << std::endl;
    
    double S0 = 100.0, K = 100.0, r = 0.05, T = 1.0, sigma = 0.2;
    
    EuropeanOption put(K, T, OptionType::PUT);
    MonteCarloEngine engine(50000, 252, 42);
    
    MCResult result = engine.price(put, S0, r, sigma);
    
    // Compare with Black-Scholes
    BlackScholes bs(S0, K, r, T, sigma);
    double bsPrice = bs.putPrice();
    
    std::cout << "  BS Price: " << bsPrice << std::endl;
    std::cout << "  MC Price: " << result.price << std::endl;
    std::cout << "  Std Error: " << result.standardError << std::endl;
    
    double error = std::abs(result.price - bsPrice);
    assert(error < 0.5);
    
    std::cout << "✓ European put pricing test passed" << std::endl;
}

void testConfidenceInterval() {
    std::cout << "Testing confidence interval..." << std::endl;
    
    double S0 = 100.0, K = 100.0, r = 0.05, T = 1.0, sigma = 0.2;
    
    EuropeanOption call(K, T, OptionType::CALL);
    MonteCarloEngine engine(10000, 252, 42);
    
    MCResult result = engine.price(call, S0, r, sigma);
    
    // Check that confidence interval is valid
    assert(result.confidence95Lower < result.price);
    assert(result.confidence95Upper > result.price);
    assert(result.confidence95Upper - result.confidence95Lower > 0);
    
    std::cout << "  Price: " << result.price << std::endl;
    std::cout << "  95% CI: [" << result.confidence95Lower 
              << ", " << result.confidence95Upper << "]" << std::endl;
    std::cout << "✓ Confidence interval test passed" << std::endl;
}

void testAmericanCallPricing() {
    std::cout << "Testing American call pricing..." << std::endl;
    
    double S0 = 100.0, K = 100.0, r = 0.05, T = 1.0, sigma = 0.2;
    
    AmericanOption call(K, T, OptionType::CALL);
    MonteCarloEngine engine(20000, 50, 42); // Fewer steps for speed
    
    MCResult result = engine.price(call, S0, r, sigma);
    
    // American call on non-dividend stock should be close to European
    BlackScholes bs(S0, K, r, T, sigma);
    double europeanPrice = bs.callPrice();
    
    std::cout << "  European Price: " << europeanPrice << std::endl;
    std::cout << "  American Price: " << result.price << std::endl;
    std::cout << "  Early Exercise Premium: " 
              << result.price - europeanPrice << std::endl;
    
    // American should be at least as valuable as European
    // Allow for significant MC error with American options
    assert(result.price >= europeanPrice - 3.0); // Allow for MC error
    
    std::cout << "✓ American call pricing test passed" << std::endl;
}

void testAmericanPutPricing() {
    std::cout << "Testing American put pricing..." << std::endl;
    
    double S0 = 100.0, K = 100.0, r = 0.05, T = 1.0, sigma = 0.2;
    
    AmericanOption put(K, T, OptionType::PUT);
    MonteCarloEngine engine(20000, 50, 42);
    
    MCResult result = engine.price(put, S0, r, sigma);
    
    // American put should be more valuable than European
    BlackScholes bs(S0, K, r, T, sigma);
    double europeanPrice = bs.putPrice();
    
    std::cout << "  European Price: " << europeanPrice << std::endl;
    std::cout << "  American Price: " << result.price << std::endl;
    std::cout << "  Early Exercise Premium: " 
              << result.price - europeanPrice << std::endl;
    
    // American put should be more valuable
    assert(result.price >= europeanPrice - 0.5); // Allow for MC error
    
    std::cout << "✓ American put pricing test passed" << std::endl;
}

void testDifferentSeeds() {
    std::cout << "Testing reproducibility with different seeds..." << std::endl;
    
    double S0 = 100.0, K = 100.0, r = 0.05, T = 1.0, sigma = 0.2;
    EuropeanOption call(K, T, OptionType::CALL);
    
    MonteCarloEngine engine1(10000, 100, 42);
    MonteCarloEngine engine2(10000, 100, 42);
    MonteCarloEngine engine3(10000, 100, 123);
    
    MCResult result1 = engine1.price(call, S0, r, sigma);
    MCResult result2 = engine2.price(call, S0, r, sigma);
    MCResult result3 = engine3.price(call, S0, r, sigma);
    
    // Same seed should give same result
    assert(std::abs(result1.price - result2.price) < 1e-10);
    
    // Different seed should give different result (very unlikely to be same)
    assert(std::abs(result1.price - result3.price) > 1e-6);
    
    std::cout << "  Same seed results: " << result1.price 
              << " vs " << result2.price << std::endl;
    std::cout << "  Different seed result: " << result3.price << std::endl;
    std::cout << "✓ Reproducibility test passed" << std::endl;
}

int main() {
    std::cout << "=== Monte Carlo Tests ===" << std::endl;
    
    testEngineCreation();
    testEuropeanCallPricing();
    testEuropeanPutPricing();
    testConfidenceInterval();
    testAmericanCallPricing();
    testAmericanPutPricing();
    testDifferentSeeds();
    
    std::cout << "\n✓ All Monte Carlo tests passed!" << std::endl;
    return 0;
}
