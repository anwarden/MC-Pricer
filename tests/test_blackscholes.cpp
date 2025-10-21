#include "BlackScholes.h"
#include <iostream>
#include <cassert>
#include <cmath>

using namespace MCPricer;

void testBlackScholesCreation() {
    std::cout << "Testing Black-Scholes creation..." << std::endl;
    
    BlackScholes bs(100.0, 100.0, 0.05, 1.0, 0.2);
    
    std::cout << "✓ Black-Scholes creation test passed" << std::endl;
}

void testCallPrice() {
    std::cout << "Testing call price calculation..." << std::endl;
    
    // ATM call option
    BlackScholes bs(100.0, 100.0, 0.05, 1.0, 0.2);
    double callPrice = bs.callPrice();
    
    // Expected value ~10.45 (approximate)
    assert(callPrice > 8.0 && callPrice < 12.0);
    assert(callPrice > 0);
    
    std::cout << "  Call price: " << callPrice << std::endl;
    std::cout << "✓ Call price test passed" << std::endl;
}

void testPutPrice() {
    std::cout << "Testing put price calculation..." << std::endl;
    
    BlackScholes bs(100.0, 100.0, 0.05, 1.0, 0.2);
    double putPrice = bs.putPrice();
    
    // Put price should be positive for ATM option
    assert(putPrice > 4.0 && putPrice < 8.0);
    
    std::cout << "  Put price: " << putPrice << std::endl;
    std::cout << "✓ Put price test passed" << std::endl;
}

void testPutCallParity() {
    std::cout << "Testing put-call parity..." << std::endl;
    
    double S = 100.0, K = 100.0, r = 0.05, T = 1.0, sigma = 0.2;
    BlackScholes bs(S, K, r, T, sigma);
    
    double callPrice = bs.callPrice();
    double putPrice = bs.putPrice();
    
    // Put-Call Parity: C - P = S - K*e^(-rT)
    double lhs = callPrice - putPrice;
    double rhs = S - K * std::exp(-r * T);
    
    assert(std::abs(lhs - rhs) < 1e-8);
    
    std::cout << "  C - P = " << lhs << std::endl;
    std::cout << "  S - K*e^(-rT) = " << rhs << std::endl;
    std::cout << "✓ Put-call parity test passed" << std::endl;
}

void testGreeks() {
    std::cout << "Testing Greeks calculation..." << std::endl;
    
    BlackScholes bs(100.0, 100.0, 0.05, 1.0, 0.2);
    
    // Delta should be between 0 and 1 for call
    double callDelta = bs.delta(OptionType::CALL);
    assert(callDelta > 0 && callDelta < 1);
    
    // Delta should be between -1 and 0 for put
    double putDelta = bs.delta(OptionType::PUT);
    assert(putDelta > -1 && putDelta < 0);
    
    // Gamma should be positive
    double gamma = bs.gamma();
    assert(gamma > 0);
    
    // Vega should be positive
    double vega = bs.vega();
    assert(vega > 0);
    
    std::cout << "  Call Delta: " << callDelta << std::endl;
    std::cout << "  Put Delta: " << putDelta << std::endl;
    std::cout << "  Gamma: " << gamma << std::endl;
    std::cout << "  Vega: " << vega << std::endl;
    std::cout << "✓ Greeks test passed" << std::endl;
}

void testImpliedVolatility() {
    std::cout << "Testing implied volatility..." << std::endl;
    
    double S = 100.0, K = 100.0, r = 0.05, T = 1.0, sigma = 0.2;
    BlackScholes bs(S, K, r, T, sigma);
    
    double marketPrice = bs.callPrice();
    double impliedVol = BlackScholes::impliedVolatility(marketPrice, S, K, r, T, 
                                                        OptionType::CALL);
    
    // Should recover the original volatility
    assert(std::abs(impliedVol - sigma) < 1e-4);
    
    std::cout << "  Original vol: " << sigma << std::endl;
    std::cout << "  Implied vol: " << impliedVol << std::endl;
    std::cout << "✓ Implied volatility test passed" << std::endl;
}

void testMoneyness() {
    std::cout << "Testing different moneyness..." << std::endl;
    
    // ITM call (S > K)
    BlackScholes bsITM(110.0, 100.0, 0.05, 1.0, 0.2);
    double itmCall = bsITM.callPrice();
    assert(itmCall > 10.0); // At least intrinsic value
    
    // OTM call (S < K)
    BlackScholes bsOTM(90.0, 100.0, 0.05, 1.0, 0.2);
    double otmCall = bsOTM.callPrice();
    assert(otmCall > 0 && otmCall < 7.0); // Time value only
    
    std::cout << "  ITM Call: " << itmCall << std::endl;
    std::cout << "  OTM Call: " << otmCall << std::endl;
    std::cout << "✓ Moneyness test passed" << std::endl;
}

int main() {
    std::cout << "=== Black-Scholes Tests ===" << std::endl;
    
    testBlackScholesCreation();
    testCallPrice();
    testPutPrice();
    testPutCallParity();
    testGreeks();
    testImpliedVolatility();
    testMoneyness();
    
    std::cout << "\n✓ All Black-Scholes tests passed!" << std::endl;
    return 0;
}
