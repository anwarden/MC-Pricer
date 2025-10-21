#pragma once
#include "option.h"

namespace aemps {

class BlackScholes {
public:
    // Returns Black-Scholes price for European option
    // volatility is annualized std dev
    static double price(const Option& opt, double rate, double volatility);
};

}
