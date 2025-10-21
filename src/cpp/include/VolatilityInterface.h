#ifndef VOLATILITY_INTERFACE_H
#define VOLATILITY_INTERFACE_H

#include <string>
#include <vector>

/**
 * @brief Interface for volatility prediction
 * 
 * Can use either historical volatility or ML-based forecasting
 */
class VolatilityInterface {
public:
    virtual ~VolatilityInterface() = default;

    /**
     * @brief Calculate or predict volatility
     * @param historicalPrices Vector of historical prices
     * @return Predicted volatility (annualized)
     */
    virtual double predictVolatility(const std::vector<double>& historicalPrices) const = 0;

    /**
     * @brief Load ML model from file (if applicable)
     * @param modelPath Path to the model file
     * @return true if successful
     */
    virtual bool loadModel(const std::string& modelPath) { 
        (void)modelPath; // Suppress unused parameter warning
        return false; 
    }
};

/**
 * @brief Historical volatility calculator
 */
class HistoricalVolatility : public VolatilityInterface {
private:
    int windowSize;
    int annualizationFactor; // typically 252 for daily data

public:
    HistoricalVolatility(int window = 30, int annualization = 252);
    double predictVolatility(const std::vector<double>& historicalPrices) const override;
};

#endif // VOLATILITY_INTERFACE_H
