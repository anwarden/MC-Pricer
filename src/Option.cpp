#include "Option.h"
#include <algorithm>
#include <stdexcept>

namespace MCPricer {

Option::Option(double strike, double maturity, OptionType type, ExerciseStyle style)
    : strike_(strike), maturity_(maturity), type_(type), style_(style) {
    if (strike <= 0) {
        throw std::invalid_argument("Strike price must be positive");
    }
    if (maturity <= 0) {
        throw std::invalid_argument("Maturity must be positive");
    }
}

EuropeanOption::EuropeanOption(double strike, double maturity, OptionType type)
    : Option(strike, maturity, type, ExerciseStyle::EUROPEAN) {}

double EuropeanOption::payoff(double spot) const {
    if (type_ == OptionType::CALL) {
        return std::max(spot - strike_, 0.0);
    } else {
        return std::max(strike_ - spot, 0.0);
    }
}

AmericanOption::AmericanOption(double strike, double maturity, OptionType type)
    : Option(strike, maturity, type, ExerciseStyle::AMERICAN) {}

double AmericanOption::payoff(double spot) const {
    // Payoff is same as European, but exercise decision is different
    if (type_ == OptionType::CALL) {
        return std::max(spot - strike_, 0.0);
    } else {
        return std::max(strike_ - spot, 0.0);
    }
}

} // namespace MCPricer
