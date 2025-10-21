#include "Option.h"
#include <iostream>
#include <cassert>
#include <cmath>

using namespace MCPricer;

void testOptionCreation() {
    std::cout << "Testing option creation..." << std::endl;
    
    EuropeanOption call(100.0, 1.0, OptionType::CALL);
    assert(call.getStrike() == 100.0);
    assert(call.getMaturity() == 1.0);
    assert(call.getType() == OptionType::CALL);
    assert(call.getStyle() == ExerciseStyle::EUROPEAN);
    
    EuropeanOption put(110.0, 0.5, OptionType::PUT);
    assert(put.getStrike() == 110.0);
    assert(put.getMaturity() == 0.5);
    assert(put.getType() == OptionType::PUT);
    
    std::cout << "✓ Option creation test passed" << std::endl;
}

void testCallPayoff() {
    std::cout << "Testing call option payoff..." << std::endl;
    
    EuropeanOption call(100.0, 1.0, OptionType::CALL);
    
    assert(std::abs(call.payoff(110.0) - 10.0) < 1e-10);
    assert(std::abs(call.payoff(100.0) - 0.0) < 1e-10);
    assert(std::abs(call.payoff(90.0) - 0.0) < 1e-10);
    
    std::cout << "✓ Call payoff test passed" << std::endl;
}

void testPutPayoff() {
    std::cout << "Testing put option payoff..." << std::endl;
    
    EuropeanOption put(100.0, 1.0, OptionType::PUT);
    
    assert(std::abs(put.payoff(90.0) - 10.0) < 1e-10);
    assert(std::abs(put.payoff(100.0) - 0.0) < 1e-10);
    assert(std::abs(put.payoff(110.0) - 0.0) < 1e-10);
    
    std::cout << "✓ Put payoff test passed" << std::endl;
}

void testAmericanOption() {
    std::cout << "Testing American option..." << std::endl;
    
    AmericanOption call(100.0, 1.0, OptionType::CALL);
    assert(call.getStyle() == ExerciseStyle::AMERICAN);
    assert(std::abs(call.payoff(110.0) - 10.0) < 1e-10);
    
    AmericanOption put(100.0, 1.0, OptionType::PUT);
    assert(std::abs(put.payoff(90.0) - 10.0) < 1e-10);
    
    std::cout << "✓ American option test passed" << std::endl;
}

void testInvalidParameters() {
    std::cout << "Testing invalid parameters..." << std::endl;
    
    try {
        EuropeanOption invalid(-100.0, 1.0, OptionType::CALL);
        assert(false); // Should not reach here
    } catch (const std::invalid_argument&) {
        // Expected
    }
    
    try {
        EuropeanOption invalid(100.0, -1.0, OptionType::CALL);
        assert(false); // Should not reach here
    } catch (const std::invalid_argument&) {
        // Expected
    }
    
    std::cout << "✓ Invalid parameters test passed" << std::endl;
}

int main() {
    std::cout << "=== Option Tests ===" << std::endl;
    
    testOptionCreation();
    testCallPayoff();
    testPutPayoff();
    testAmericanOption();
    testInvalidParameters();
    
    std::cout << "\n✓ All option tests passed!" << std::endl;
    return 0;
}
