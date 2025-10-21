#include "VolatilityPredictor.h"
#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <random>

using namespace MCPricer;

std::vector<double> generateReturns(int n, double mean, double stddev, unsigned int seed) {
    std::mt19937 gen(seed);
    std::normal_distribution<double> dist(mean, stddev);
    
    std::vector<double> returns;
    for (int i = 0; i < n; ++i) {
        returns.push_back(dist(gen));
    }
    return returns;
}

void testHistoricalVolatility() {
    std::cout << "Testing historical volatility..." << std::endl;
    
    // Generate returns with known volatility
    double dailyVol = 0.01;
    std::vector<double> returns = generateReturns(100, 0.0, dailyVol, 42);
    
    HistoricalVolatility histVol(30);
    double predictedVol = histVol.predict(returns);
    
    // Should be close to true annualized volatility
    double trueAnnualVol = dailyVol * std::sqrt(252.0);
    
    std::cout << "  True vol: " << trueAnnualVol << std::endl;
    std::cout << "  Predicted vol: " << predictedVol << std::endl;
    
    // Allow for sampling error
    assert(std::abs(predictedVol - trueAnnualVol) < 0.05);
    
    std::cout << "✓ Historical volatility test passed" << std::endl;
}

void testEWMAVolatility() {
    std::cout << "Testing EWMA volatility..." << std::endl;
    
    double dailyVol = 0.01;
    std::vector<double> returns = generateReturns(100, 0.0, dailyVol, 42);
    
    EWMAVolatility ewmaVol(0.94);
    double predictedVol = ewmaVol.predict(returns);
    
    // Should be close to true annualized volatility
    double trueAnnualVol = dailyVol * std::sqrt(252.0);
    
    std::cout << "  True vol: " << trueAnnualVol << std::endl;
    std::cout << "  Predicted vol: " << predictedVol << std::endl;
    
    assert(std::abs(predictedVol - trueAnnualVol) < 0.05);
    
    std::cout << "✓ EWMA volatility test passed" << std::endl;
}

void testDifferentLambdas() {
    std::cout << "Testing EWMA with different lambdas..." << std::endl;
    
    std::vector<double> returns = generateReturns(100, 0.0, 0.01, 42);
    
    EWMAVolatility ewma90(0.90);
    EWMAVolatility ewma94(0.94);
    EWMAVolatility ewma98(0.98);
    
    double vol90 = ewma90.predict(returns);
    double vol94 = ewma94.predict(returns);
    double vol98 = ewma98.predict(returns);
    
    std::cout << "  Lambda=0.90: " << vol90 << std::endl;
    std::cout << "  Lambda=0.94: " << vol94 << std::endl;
    std::cout << "  Lambda=0.98: " << vol98 << std::endl;
    
    // All should be positive
    assert(vol90 > 0);
    assert(vol94 > 0);
    assert(vol98 > 0);
    
    std::cout << "✓ Different lambdas test passed" << std::endl;
}

void testMLVolatilityPredictor() {
    std::cout << "Testing ML volatility predictor..." << std::endl;
    
    std::vector<double> returns = generateReturns(100, 0.0, 0.01, 42);
    
    MLVolatilityPredictor mlVol;
    double predictedVol = mlVol.predict(returns);
    
    // Should return a reasonable volatility
    assert(predictedVol > 0);
    assert(predictedVol < 2.0); // Reasonable upper bound
    
    std::cout << "  ML Predicted vol: " << predictedVol << std::endl;
    std::cout << "✓ ML volatility predictor test passed" << std::endl;
}

void testWindowSizes() {
    std::cout << "Testing different window sizes..." << std::endl;
    
    std::vector<double> returns = generateReturns(100, 0.0, 0.01, 42);
    
    HistoricalVolatility hist10(10);
    HistoricalVolatility hist30(30);
    HistoricalVolatility hist60(60);
    
    double vol10 = hist10.predict(returns);
    double vol30 = hist30.predict(returns);
    double vol60 = hist60.predict(returns);
    
    std::cout << "  Window=10: " << vol10 << std::endl;
    std::cout << "  Window=30: " << vol30 << std::endl;
    std::cout << "  Window=60: " << vol60 << std::endl;
    
    // All should be positive
    assert(vol10 > 0);
    assert(vol30 > 0);
    assert(vol60 > 0);
    
    std::cout << "✓ Different window sizes test passed" << std::endl;
}

void testChangingVolatility() {
    std::cout << "Testing changing volatility regime..." << std::endl;
    
    std::vector<double> returns;
    
    // Low volatility period
    auto lowVolReturns = generateReturns(50, 0.0, 0.005, 42);
    returns.insert(returns.end(), lowVolReturns.begin(), lowVolReturns.end());
    
    // High volatility period
    auto highVolReturns = generateReturns(50, 0.0, 0.02, 123);
    returns.insert(returns.end(), highVolReturns.begin(), highVolReturns.end());
    
    // EWMA should adapt faster than historical
    HistoricalVolatility histVol(30);
    EWMAVolatility ewmaVol(0.94);
    
    double histVolatility = histVol.predict(returns);
    double ewmaVolatility = ewmaVol.predict(returns);
    
    std::cout << "  Historical vol: " << histVolatility << std::endl;
    std::cout << "  EWMA vol: " << ewmaVolatility << std::endl;
    
    // Both should be positive
    assert(histVolatility > 0);
    assert(ewmaVolatility > 0);
    
    std::cout << "✓ Changing volatility test passed" << std::endl;
}

void testEmptyReturns() {
    std::cout << "Testing empty returns handling..." << std::endl;
    
    std::vector<double> emptyReturns;
    
    HistoricalVolatility histVol(30);
    
    try {
        histVol.predict(emptyReturns);
        assert(false); // Should throw
    } catch (const std::invalid_argument&) {
        // Expected
    }
    
    std::cout << "✓ Empty returns test passed" << std::endl;
}

int main() {
    std::cout << "=== Volatility Predictor Tests ===" << std::endl;
    
    testHistoricalVolatility();
    testEWMAVolatility();
    testDifferentLambdas();
    testMLVolatilityPredictor();
    testWindowSizes();
    testChangingVolatility();
    testEmptyReturns();
    
    std::cout << "\n✓ All volatility predictor tests passed!" << std::endl;
    return 0;
}
