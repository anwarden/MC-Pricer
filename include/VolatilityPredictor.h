#ifndef VOLATILITY_PREDICTOR_H
#define VOLATILITY_PREDICTOR_H

#include <vector>
#include <string>

namespace MCPricer {

// Interface for volatility prediction models
class VolatilityPredictor {
public:
    virtual ~VolatilityPredictor() = default;
    
    // Predict future volatility based on historical returns
    virtual double predict(const std::vector<double>& returns) = 0;
    
    // Get model name
    virtual std::string getModelName() const = 0;
};

// Historical volatility calculator
class HistoricalVolatility : public VolatilityPredictor {
private:
    int windowSize_;

public:
    explicit HistoricalVolatility(int windowSize = 30);
    double predict(const std::vector<double>& returns) override;
    std::string getModelName() const override { return "Historical"; }
};

// EWMA (Exponentially Weighted Moving Average) volatility
class EWMAVolatility : public VolatilityPredictor {
private:
    double lambda_; // Decay factor

public:
    explicit EWMAVolatility(double lambda = 0.94);
    double predict(const std::vector<double>& returns) override;
    std::string getModelName() const override { return "EWMA"; }
};

// Python ML model integration (to be connected with Python backend)
class MLVolatilityPredictor : public VolatilityPredictor {
private:
    std::string pythonModulePath_;
    bool isPythonInitialized_;

public:
    explicit MLVolatilityPredictor(const std::string& modulePath = "ml_volatility");
    double predict(const std::vector<double>& returns) override;
    std::string getModelName() const override { return "ML-Based"; }
};

} // namespace MCPricer

#endif // VOLATILITY_PREDICTOR_H
