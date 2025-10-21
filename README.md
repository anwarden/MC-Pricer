# MC-Pricer

[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![Python](https://img.shields.io/badge/Python-3.8+-green.svg)](https://www.python.org/)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

A production-grade option pricing engine that combines Black-Scholes analytics with Monte Carlo simulation and ML-based volatility forecasting. Demonstrates deep understanding of financial derivatives and computational finance.

## 🎯 Features

### ⭕ Object-Oriented C++ Architecture
- **High-performance pricing engine** with modern C++17 design patterns
- **Black-Scholes analytical model** with complete Greeks calculation
- **Monte Carlo simulation** with variance reduction techniques
- **American options pricing** using Longstaff-Schwartz algorithm
- **Implied volatility** calculation using Newton-Raphson method

### ⭕ Python ML Module
- **GARCH model** for volatility forecasting
- **EWMA (Exponentially Weighted Moving Average)** volatility estimation
- **LSTM-based** deep learning models (framework ready)
- **Ensemble methods** combining multiple forecasting models
- **Python-C++ integration** for seamless workflow

### ⭕ Integration of Classical & Modern Methods
- Bridges academic finance theory with real-world trading applications
- Combines traditional quantitative finance with modern AI/ML
- Extensible architecture for adding new models and strategies

## 🚀 Quick Start

### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.10+
- Python 3.8+ with NumPy and SciPy

### Building the Project

```bash
# Clone the repository
git clone https://github.com/anwarden/MC-Pricer.git
cd MC-Pricer

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make

# Run tests
ctest --verbose

# Run the demo
./pricing_demo
```

### Installing Python Dependencies

```bash
cd python
pip install -r requirements.txt
```

## 📊 Usage Examples

### C++ Example

```cpp
#include "BlackScholes.h"
#include "MonteCarloEngine.h"
#include "VolatilityPredictor.h"

using namespace MCPricer;

int main() {
    // Market parameters
    double S0 = 100.0;      // Spot price
    double K = 100.0;       // Strike
    double r = 0.05;        // Risk-free rate
    double T = 1.0;         // Maturity
    double sigma = 0.20;    // Volatility
    
    // Black-Scholes pricing
    BlackScholes bs(S0, K, r, T, sigma);
    double callPrice = bs.callPrice();
    double delta = bs.delta(OptionType::CALL);
    
    // Monte Carlo pricing
    EuropeanOption option(K, T, OptionType::CALL);
    MonteCarloEngine mc(100000, 252);
    MCResult result = mc.price(option, S0, r, sigma);
    
    // Volatility forecasting
    std::vector<double> returns = {...}; // Historical returns
    EWMAVolatility ewma(0.94);
    double forecastedVol = ewma.predict(returns);
    
    return 0;
}
```

### Python Example

```python
from python.option_pricing import OptionPricer, VolatilityForecaster
import numpy as np

# Volatility forecasting
returns = np.random.normal(0, 0.01, 100)
forecaster = VolatilityForecaster('garch')
volatility = forecaster.forecast(returns.tolist())

# Option pricing
pricer = OptionPricer()
call_price = pricer.black_scholes_price(
    S=100, K=100, r=0.05, T=1.0, 
    sigma=volatility, option_type='call'
)

# Monte Carlo pricing
mc_result = pricer.monte_carlo_price(
    S=100, K=100, r=0.05, T=1.0,
    sigma=volatility, option_type='call',
    num_simulations=100000
)

print(f"Call Price: ${call_price:.2f}")
print(f"MC Price: ${mc_result['price']:.2f} ± ${mc_result['standard_error']:.2f}")
```

## 🏗️ Architecture

```
MC-Pricer/
├── include/              # C++ header files
│   ├── Option.h         # Option base classes
│   ├── BlackScholes.h   # BS model and Greeks
│   ├── MonteCarloEngine.h  # MC simulation
│   └── VolatilityPredictor.h  # Volatility models
├── src/                 # C++ implementation
├── python/              # Python ML module
│   ├── ml_volatility.py    # GARCH, LSTM models
│   ├── option_pricing.py   # Python interface
│   └── requirements.txt
├── tests/               # Unit tests
├── examples/            # Example applications
└── CMakeLists.txt       # Build configuration
```

## 🔬 Mathematical Models

### Black-Scholes Formula

The Black-Scholes price for a European call option:

```
C = S₀N(d₁) - Ke⁻ʳᵀN(d₂)

where:
d₁ = [ln(S₀/K) + (r + σ²/2)T] / (σ√T)
d₂ = d₁ - σ√T
```

### Monte Carlo Simulation

Asset price paths follow geometric Brownian motion:

```
dS = μS dt + σS dW

Discretized:
S(t+Δt) = S(t) exp[(r - σ²/2)Δt + σ√Δt Z]
```

### GARCH(1,1) Model

Volatility forecasting:

```
σ²ₜ = ω + α r²ₜ₋₁ + β σ²ₜ₋₁
```

### EWMA Volatility

```
σ²ₜ = λσ²ₜ₋₁ + (1-λ)r²ₜ₋₁
```

## 🧪 Testing

The project includes comprehensive test suites:

```bash
# Run all tests
ctest --verbose

# Run specific tests
./test_option
./test_blackscholes
./test_montecarlo
./test_volatility
```

## 📈 Performance

- **Black-Scholes**: Sub-microsecond pricing
- **Monte Carlo**: 100K simulations in ~100ms (depends on CPU)
- **Optimizations**: 
  - Variance reduction techniques
  - Parallel simulation paths (future enhancement)
  - SIMD vectorization opportunities

## 🎓 Educational Value

This project demonstrates:

1. **Quantitative Finance**: Option pricing theory, risk-neutral valuation, Greeks
2. **Numerical Methods**: Monte Carlo simulation, polynomial regression
3. **Time Series**: GARCH, EWMA, volatility modeling
4. **Machine Learning**: LSTM networks, ensemble methods
5. **Software Engineering**: OOP, design patterns, testing, documentation

## 🔮 Future Enhancements

- [ ] Exotic options (Asian, Barrier, Lookback)
- [ ] Parallel Monte Carlo with OpenMP/CUDA
- [ ] Real-time market data integration
- [ ] Interactive web dashboard
- [ ] More ML models (Random Forest, XGBoost)
- [ ] Greeks calculation via Monte Carlo
- [ ] Calibration to market data

## 📚 References

- Black, F., & Scholes, M. (1973). The Pricing of Options and Corporate Liabilities
- Longstaff, F. A., & Schwartz, E. S. (2001). Valuing American Options by Simulation
- Hull, J. C. (2018). Options, Futures, and Other Derivatives
- Glasserman, P. (2004). Monte Carlo Methods in Financial Engineering

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 👤 Author

**anwarden**

This project demonstrates the integration of classical quantitative finance with modern machine learning, bridging academic theory with real-world trading applications.

## 🙏 Acknowledgments

- Inspired by production trading systems
- Built with modern C++ and Python best practices
- Designed for both education and practical application
