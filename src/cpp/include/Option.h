#ifndef OPTION_H
#define OPTION_H

#include <string>

/**
 * @brief Abstract base class for options
 * 
 * Provides interface for option pricing and Greeks calculation
 */
class Option {
protected:
    double spotPrice;      // Current price of underlying asset
    double strikePrice;    // Strike price
    double riskFreeRate;   // Risk-free interest rate
    double volatility;     // Volatility (sigma)
    double timeToMaturity; // Time to maturity in years
    std::string optionType; // "call" or "put"

public:
    Option(double S, double K, double r, double sigma, double T, const std::string& type);
    virtual ~Option() = default;

    // Pure virtual functions - must be implemented by derived classes
    virtual double calculatePrice() const = 0;
    virtual double calculateDelta() const = 0;
    virtual double calculateGamma() const = 0;
    virtual double calculateVega() const = 0;
    virtual double calculateTheta() const = 0;
    virtual double calculateRho() const = 0;

    // Getters
    double getSpotPrice() const { return spotPrice; }
    double getStrikePrice() const { return strikePrice; }
    double getRiskFreeRate() const { return riskFreeRate; }
    double getVolatility() const { return volatility; }
    double getTimeToMaturity() const { return timeToMaturity; }
    std::string getOptionType() const { return optionType; }

    // Setters
    void setVolatility(double sigma) { volatility = sigma; }
    void setSpotPrice(double S) { spotPrice = S; }
};

#endif // OPTION_H
