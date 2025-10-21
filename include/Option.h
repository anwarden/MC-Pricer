#ifndef OPTION_H
#define OPTION_H

#include <string>
#include <memory>

namespace MCPricer {

// Enum for option types
enum class OptionType {
    CALL,
    PUT
};

// Enum for exercise styles
enum class ExerciseStyle {
    EUROPEAN,
    AMERICAN
};

// Base class for all options
class Option {
protected:
    double strike_;
    double maturity_;
    OptionType type_;
    ExerciseStyle style_;

public:
    Option(double strike, double maturity, OptionType type, ExerciseStyle style);
    virtual ~Option() = default;

    // Pure virtual function for payoff calculation
    virtual double payoff(double spot) const = 0;

    // Getters
    double getStrike() const { return strike_; }
    double getMaturity() const { return maturity_; }
    OptionType getType() const { return type_; }
    ExerciseStyle getStyle() const { return style_; }
};

// European Option class
class EuropeanOption : public Option {
public:
    EuropeanOption(double strike, double maturity, OptionType type);
    double payoff(double spot) const override;
};

// American Option class
class AmericanOption : public Option {
public:
    AmericanOption(double strike, double maturity, OptionType type);
    double payoff(double spot) const override;
};

} // namespace MCPricer

#endif // OPTION_H
