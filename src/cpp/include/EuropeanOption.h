#ifndef EUROPEAN_OPTION_H
#define EUROPEAN_OPTION_H

#include "Option.h"

/**
 * @brief European option implementation using Black-Scholes formula
 */
class EuropeanOption : public Option {
public:
    EuropeanOption(double S, double K, double r, double sigma, double T, const std::string& type);

    // Black-Scholes analytical pricing
    double calculatePrice() const override;
    double calculateDelta() const override;
    double calculateGamma() const override;
    double calculateVega() const override;
    double calculateTheta() const override;
    double calculateRho() const override;

private:
    // Helper functions for Black-Scholes
    double d1() const;
    double d2() const;
    double normalCDF(double x) const;
    double normalPDF(double x) const;
};

#endif // EUROPEAN_OPTION_H
