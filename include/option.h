#pragma once
#include <string>

namespace aemps {

enum class OptionType { Call, Put };

struct Option {
    OptionType type;
    double strike;
    double maturity; // in years
    double spot;

    Option(OptionType t, double k, double T, double S)
        : type(t), strike(k), maturity(T), spot(S) {}
};

}
