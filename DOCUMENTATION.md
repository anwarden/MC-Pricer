# Documentation

## Table of Contents
1. [Installation](#installation)
2. [API Reference](#api-reference)
3. [Examples](#examples)
4. [Theory](#theory)

## Installation

### Linux/MacOS

```bash
# Install dependencies
sudo apt-get install cmake g++ python3 python3-pip  # Ubuntu/Debian
# or
brew install cmake python  # MacOS

# Build the project
mkdir build && cd build
cmake ..
make -j$(nproc)

# Run tests
ctest --verbose

# Install Python dependencies
cd ../python
pip install -r requirements.txt
```

### Windows

```bash
# Using Visual Studio
mkdir build && cd build
cmake .. -G "Visual Studio 16 2019"
cmake --build . --config Release

# Or using MinGW
cmake .. -G "MinGW Makefiles"
make
```

## API Reference

### C++ Classes

#### Option Classes

**Base Class: `Option`**
```cpp
class Option {
    double getStrike() const;
    double getMaturity() const;
    OptionType getType() const;
    ExerciseStyle getStyle() const;
    virtual double payoff(double spot) const = 0;
};
```

**European Option**
```cpp
EuropeanOption(double strike, double maturity, OptionType type);
```

**American Option**
```cpp
AmericanOption(double strike, double maturity, OptionType type);
```

#### BlackScholes Class

```cpp
class BlackScholes {
public:
    BlackScholes(double S, double K, double r, double T, double sigma);
    
    // Pricing
    double callPrice() const;
    double putPrice() const;
    double price(OptionType type) const;
    
    // Greeks
    double delta(OptionType type) const;
    double gamma() const;
    double vega() const;
    double theta(OptionType type) const;
    double rho(OptionType type) const;
    
    // Implied volatility
    static double impliedVolatility(
        double marketPrice, double S, double K, 
        double r, double T, OptionType type,
        double tolerance = 1e-6, int maxIterations = 100
    );
};
```

#### MonteCarloEngine Class

```cpp
class MonteCarloEngine {
public:
    MonteCarloEngine(int numSimulations = 100000, 
                     int numSteps = 252, 
                     unsigned int seed = 42);
    
    MCResult price(const Option& option, 
                   double S0, double r, double sigma) const;
    
    void setNumSimulations(int n);
    void setNumSteps(int n);
};

struct MCResult {
    double price;
    double standardError;
    double confidence95Lower;
    double confidence95Upper;
    int numSimulations;
};
```

#### VolatilityPredictor Classes

```cpp
// Base class
class VolatilityPredictor {
public:
    virtual double predict(const std::vector<double>& returns) = 0;
    virtual std::string getModelName() const = 0;
};

// Historical volatility
class HistoricalVolatility : public VolatilityPredictor {
public:
    explicit HistoricalVolatility(int windowSize = 30);
    double predict(const std::vector<double>& returns) override;
};

// EWMA volatility
class EWMAVolatility : public VolatilityPredictor {
public:
    explicit EWMAVolatility(double lambda = 0.94);
    double predict(const std::vector<double>& returns) override;
};

// ML-based volatility
class MLVolatilityPredictor : public VolatilityPredictor {
public:
    explicit MLVolatilityPredictor(const std::string& modulePath);
    double predict(const std::vector<double>& returns) override;
};
```

### Python API

#### OptionPricer Class

```python
class OptionPricer:
    def black_scholes_price(self, S, K, r, T, sigma, option_type='call'):
        """Calculate Black-Scholes price"""
        
    def monte_carlo_price(self, S, K, r, T, sigma, 
                         option_type='call',
                         num_simulations=100000, 
                         num_steps=252):
        """Price using Monte Carlo simulation"""
```

#### VolatilityForecaster Class

```python
class VolatilityForecaster:
    def __init__(self, model_type='garch'):
        """Initialize with model type: 'historical', 'ewma', 'garch', 'lstm', 'ensemble'"""
    
    def forecast(self, returns):
        """Forecast volatility"""
        
    def forecast_multiple(self, returns, models=None):
        """Forecast using multiple models"""
```

#### Standalone Functions

```python
def predict_volatility(returns, model='garch'):
    """Predict volatility using specified model"""

def calculate_historical_volatility(returns, window=30):
    """Calculate historical volatility"""
    
def calculate_ewma_volatility(returns, lambda_param=0.94):
    """Calculate EWMA volatility"""
```

## Examples

### Example 1: Basic Option Pricing

```cpp
#include "BlackScholes.h"
#include "Option.h"

using namespace MCPricer;

int main() {
    // ATM European call option
    double S = 100.0, K = 100.0, r = 0.05, T = 1.0, sigma = 0.2;
    
    BlackScholes bs(S, K, r, T, sigma);
    double price = bs.callPrice();
    double delta = bs.delta(OptionType::CALL);
    
    std::cout << "Call price: " << price << std::endl;
    std::cout << "Delta: " << delta << std::endl;
    
    return 0;
}
```

### Example 2: Monte Carlo Simulation

```cpp
#include "MonteCarloEngine.h"
#include "Option.h"

using namespace MCPricer;

int main() {
    // Create option
    EuropeanOption call(100.0, 1.0, OptionType::CALL);
    
    // Create MC engine
    MonteCarloEngine mc(100000, 252);
    
    // Price the option
    MCResult result = mc.price(call, 100.0, 0.05, 0.2);
    
    std::cout << "Price: " << result.price << std::endl;
    std::cout << "SE: " << result.standardError << std::endl;
    
    return 0;
}
```

### Example 3: Volatility Forecasting

```cpp
#include "VolatilityPredictor.h"
#include <vector>

using namespace MCPricer;

int main() {
    std::vector<double> returns = {...}; // Historical returns
    
    // Try different models
    HistoricalVolatility hist(30);
    EWMAVolatility ewma(0.94);
    
    double histVol = hist.predict(returns);
    double ewmaVol = ewma.predict(returns);
    
    std::cout << "Historical: " << histVol << std::endl;
    std::cout << "EWMA: " << ewmaVol << std::endl;
    
    return 0;
}
```

### Example 4: Python Integration

```python
from python.option_pricing import OptionPricer, VolatilityForecaster
import numpy as np

# Generate sample data
returns = np.random.normal(0, 0.01, 100)

# Forecast volatility
forecaster = VolatilityForecaster('garch')
vol = forecaster.forecast(returns.tolist())

# Price option
pricer = OptionPricer()
price = pricer.black_scholes_price(
    S=100, K=100, r=0.05, T=1.0, 
    sigma=vol, option_type='call'
)

print(f"Forecasted vol: {vol:.4f}")
print(f"Call price: ${price:.2f}")
```

## Theory

### Black-Scholes Model

The Black-Scholes model assumes:
- Log-normal distribution of stock prices
- Constant volatility and interest rate
- No dividends
- European exercise
- No transaction costs

**Call Price:**
```
C = S₀N(d₁) - Ke⁻ʳᵀN(d₂)
```

**Put Price:**
```
P = Ke⁻ʳᵀN(-d₂) - S₀N(-d₁)
```

Where:
```
d₁ = [ln(S₀/K) + (r + σ²/2)T] / (σ√T)
d₂ = d₁ - σ√T
```

### Greeks

- **Delta (Δ)**: Rate of change of option price with respect to underlying
- **Gamma (Γ)**: Rate of change of delta with respect to underlying
- **Vega (ν)**: Sensitivity to volatility changes
- **Theta (Θ)**: Time decay of option value
- **Rho (ρ)**: Sensitivity to interest rate changes

### Monte Carlo Simulation

1. Generate random price paths using geometric Brownian motion
2. Calculate payoff for each path
3. Discount and average payoffs
4. Estimate standard error and confidence intervals

**Advantages:**
- Handles path-dependent options
- Flexible for complex payoffs
- Parallelizable

**Disadvantages:**
- Computationally intensive
- Convergence rate O(1/√N)

### American Options (Longstaff-Schwartz)

1. Simulate price paths forward
2. Backward recursion from maturity
3. At each time step:
   - Regression to estimate continuation value
   - Compare with exercise value
   - Update cash flows

### Volatility Models

**Historical Volatility:**
- Simple standard deviation of returns
- Window-based calculation
- Assumes constant volatility

**EWMA:**
- Exponentially weighted moving average
- More weight on recent observations
- Parameter λ controls decay

**GARCH(1,1):**
- Models volatility clustering
- Captures mean reversion
- Maximum likelihood estimation

**ML Models:**
- LSTM for sequential patterns
- Non-linear relationships
- Feature engineering opportunities

### Risk-Neutral Valuation

Options are priced under risk-neutral measure where:
- Expected return on stock = risk-free rate
- Discount at risk-free rate
- No arbitrage principle

### Put-Call Parity

For European options:
```
C - P = S₀ - Ke⁻ʳᵀ
```

This relationship must hold to prevent arbitrage.
