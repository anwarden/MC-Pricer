# MC-Pricer Technical Guide

## Architecture Overview

MC-Pricer implements a production-grade option pricing system using a hybrid architecture that combines:

1. **C++ Core Engine** - High-performance pricing algorithms
2. **Python ML Module** - Advanced volatility forecasting
3. **Integration Layer** - Seamless communication between components

### Component Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     MC-Pricer System                         │
├─────────────────────────────────────────────────────────────┤
│                                                               │
│  ┌──────────────────┐         ┌──────────────────┐          │
│  │   C++ Engine     │         │  Python ML       │          │
│  │                  │         │  Module          │          │
│  │  • Option Base   │         │                  │          │
│  │  • Black-Scholes │◄────────┤  • GARCH         │          │
│  │  • Monte Carlo   │ Config  │  • LSTM          │          │
│  │  • Greeks        │ Files   │  • Historical    │          │
│  │                  │         │    Volatility    │          │
│  └──────────────────┘         └──────────────────┘          │
│           │                            │                      │
│           └────────────┬───────────────┘                      │
│                        │                                      │
│                   ┌────▼────┐                                │
│                   │  User   │                                 │
│                   │Interface│                                 │
│                   └─────────┘                                 │
└─────────────────────────────────────────────────────────────┘
```

## C++ Component Details

### Class Hierarchy

```
Option (Abstract Base)
    ├── EuropeanOption
    └── (Extensible for AmericanOption, AsianOption, etc.)

MonteCarloEngine (Standalone)
    └── Geometric Brownian Motion Simulator

VolatilityInterface (Abstract Base)
    └── HistoricalVolatility
```

### Key Features

**Option Base Class**
- Provides common interface for all option types
- Encapsulates market parameters (S, K, r, σ, T)
- Defines pure virtual methods for pricing and Greeks

**EuropeanOption Implementation**
- Analytical Black-Scholes pricing
- Complete Greeks calculation (Delta, Gamma, Vega, Theta, Rho)
- Both call and put options supported
- Put-call parity validated

**Monte Carlo Engine**
- Configurable simulation parameters
- Uses Mersenne Twister RNG for quality
- Returns price estimate with standard error
- Thread-safe design (can be parallelized)

### Performance Characteristics

| Operation | Typical Time |
|-----------|--------------|
| Black-Scholes Pricing | < 1 μs |
| Greeks Calculation | < 5 μs |
| Monte Carlo (100k simulations) | ~100 ms |
| Historical Volatility | < 1 ms |

## Python ML Component Details

### Volatility Prediction Models

**1. Historical Volatility**
```python
σ = sqrt(var(log_returns) * annualization_factor)
```
- Simple, fast, widely used
- Based on realized volatility
- Good baseline for comparison

**2. GARCH(1,1) Model**
```python
σ²ₜ = ω + α·ε²ₜ₋₁ + β·σ²ₜ₋₁
```
- Captures volatility clustering
- Time-series forecasting
- Proven in financial applications
- Parameters:
  - ω: Long-term variance
  - α: ARCH coefficient (short-term shocks)
  - β: GARCH coefficient (persistence)

**3. LSTM Neural Network** (Optional)
```python
# Deep learning architecture
LSTM(50) -> Dropout(0.2) -> LSTM(25) -> Dropout(0.2) -> Dense(1)
```
- Learns complex patterns
- Handles non-linear relationships
- Requires more data and computation
- Optional TensorFlow dependency

### Model Comparison

| Model | Pros | Cons | Use Case |
|-------|------|------|----------|
| Historical | Fast, simple, interpretable | Backward-looking, assumes stationarity | Quick estimates, baseline |
| GARCH | Captures clustering, proven | Requires stationarity, parameter estimation | Standard production use |
| LSTM | Flexible, non-linear | Black box, needs data, slower | Advanced forecasting |

## Integration Methods

### Method 1: Configuration File (Implemented)

Python writes volatility to config file:
```python
# Python side
with open('volatility_config.txt', 'w') as f:
    f.write(f"volatility={predicted_vol:.6f}\n")
```

C++ reads config and prices:
```cpp
// C++ side (to be implemented)
double vol = read_config("volatility_config.txt", "volatility");
EuropeanOption option(S, K, r, vol, T, "call");
double price = option.calculatePrice();
```

### Method 2: Subprocess (Implemented in examples)

```python
import subprocess
result = subprocess.run(['./mcpricer', str(volatility)], capture_output=True)
```

### Method 3: Python Bindings (Future Enhancement)

Using pybind11:
```cpp
#include <pybind11/pybind11.h>

PYBIND11_MODULE(mcpricer_cpp, m) {
    py::class_<EuropeanOption>(m, "EuropeanOption")
        .def(py::init<double, double, double, double, double, std::string>())
        .def("calculatePrice", &EuropeanOption::calculatePrice);
}
```

Then in Python:
```python
import mcpricer_cpp
option = mcpricer_cpp.EuropeanOption(S, K, r, vol, T, "call")
price = option.calculatePrice()
```

## Financial Mathematics Background

### Black-Scholes Model

**Assumptions:**
1. Frictionless markets (no transaction costs)
2. Continuous trading
3. Constant risk-free rate and volatility
4. Log-normal price distribution
5. No dividends
6. European exercise only

**Key Equations:**

Call Option Price:
```
C = S₀N(d₁) - Ke⁻ʳᵀN(d₂)

where:
d₁ = [ln(S₀/K) + (r + σ²/2)T] / (σ√T)
d₂ = d₁ - σ√T
N(x) = standard normal CDF
```

Put Option Price:
```
P = Ke⁻ʳᵀN(-d₂) - S₀N(-d₁)
```

**Greeks:**

| Greek | Formula | Interpretation |
|-------|---------|----------------|
| Delta (Δ) | ∂C/∂S | Price sensitivity to spot |
| Gamma (Γ) | ∂²C/∂S² | Delta sensitivity to spot |
| Vega (ν) | ∂C/∂σ | Price sensitivity to volatility |
| Theta (Θ) | ∂C/∂t | Price decay over time |
| Rho (ρ) | ∂C/∂r | Price sensitivity to rates |

### Monte Carlo Simulation

**Process:**

1. Simulate asset price paths using Geometric Brownian Motion:
   ```
   S(t+Δt) = S(t) · exp[(r - σ²/2)Δt + σ√Δt·Z]
   where Z ~ N(0,1)
   ```

2. Calculate payoff for each path:
   ```
   Payoff = max(Sₜ - K, 0)  for call
   Payoff = max(K - Sₜ, 0)  for put
   ```

3. Average and discount:
   ```
   Price ≈ e⁻ʳᵀ · (1/N) · Σ Payoffs
   ```

**Convergence:**
- Error decreases as O(1/√N)
- Standard error = σ/√N
- Variance reduction techniques can improve efficiency

### GARCH Model Theory

**Volatility Dynamics:**

The GARCH(1,1) model captures:
- **Volatility Clustering**: Large changes tend to follow large changes
- **Mean Reversion**: Volatility reverts to long-run average
- **Asymmetry**: Can be extended to GJR-GARCH for leverage effects

**Parameter Interpretation:**
- α + β < 1: Stationarity condition
- α: Sensitivity to recent shocks
- β: Persistence of volatility
- ω/(1-α-β): Long-run variance

## Usage Patterns

### Basic Option Pricing (C++)

```cpp
#include "EuropeanOption.h"

// Create option
EuropeanOption call(100.0, 105.0, 0.05, 0.25, 1.0, "call");

// Price and Greeks
double price = call.calculatePrice();
double delta = call.calculateDelta();
double gamma = call.calculateGamma();
```

### Monte Carlo Pricing (C++)

```cpp
#include "MonteCarloEngine.h"

MonteCarloEngine mc(100000, 252, 42);
auto [price, error] = mc.priceEuropeanOption(option);
std::cout << "Price: " << price << " ± " << error << std::endl;
```

### Volatility Prediction (Python)

```python
from ml_volatility import GARCHPredictor

# Train model
garch = GARCHPredictor(lookback_window=60)
garch.fit(historical_prices)

# Predict
vol = garch.predict(recent_prices)
```

### Complete Workflow

```python
# 1. Get historical data
prices = load_historical_data('data.csv')

# 2. Predict volatility
garch = GARCHPredictor()
garch.fit(prices)
vol = garch.predict(prices[-60:])

# 3. Write config
with open('config.txt', 'w') as f:
    f.write(f"volatility={vol}\n")

# 4. Call C++ pricer
subprocess.run(['./mcpricer'])
```

## Extension Points

### Adding New Option Types

```cpp
class AmericanOption : public Option {
public:
    double calculatePrice() const override {
        // Implement binomial tree or LSM
    }
    // ... implement Greeks
};
```

### Adding New Volatility Models

```python
class EWMAPredictor(VolatilityPredictor):
    def fit(self, prices):
        # Implement exponentially weighted moving average
        pass
    
    def predict(self, prices):
        # Return EWMA volatility
        pass
```

### Parallel Monte Carlo

```cpp
// Using OpenMP
#pragma omp parallel for
for (int i = 0; i < numSimulations; ++i) {
    // Simulate path
    // Calculate payoff
}
```

## Testing Strategy

### Unit Tests (C++)
- Option pricing correctness
- Put-call parity validation
- Greeks accuracy
- Monte Carlo convergence
- Edge cases (T=0, extreme volatility)

### Unit Tests (Python)
- Volatility calculations
- Model fitting
- Prediction accuracy
- Data preprocessing
- Error handling

### Integration Tests
- Config file I/O
- Python-C++ communication
- End-to-end workflow
- Performance benchmarks

## Performance Optimization Tips

1. **C++ Optimizations:**
   - Use `-O3` compiler flag
   - Consider SSE/AVX for vectorization
   - Profile with `gprof` or `perf`
   - Cache frequently used values (d1, d2)

2. **Monte Carlo:**
   - Use parallel processing (OpenMP)
   - Implement variance reduction (antithetic variates)
   - Consider quasi-random sequences (Sobol)

3. **Python:**
   - Use NumPy vectorization
   - Batch predictions
   - Consider Numba JIT compilation
   - Profile with `cProfile`

## Common Issues and Solutions

**Issue:** Monte Carlo price doesn't match Black-Scholes
- Solution: Increase number of simulations
- Check: Risk-free rate and time step calculations

**Issue:** GARCH model gives unrealistic volatility
- Solution: Check data quality, ensure enough history
- Consider: Different parameter initialization

**Issue:** Build fails on Windows
- Solution: Use Visual Studio CMake generator
- Check: C++17 support in compiler

## References and Further Reading

**Books:**
- Hull, J. "Options, Futures, and Other Derivatives"
- Glasserman, P. "Monte Carlo Methods in Financial Engineering"
- Tsay, R. "Analysis of Financial Time Series"

**Papers:**
- Black-Scholes (1973) - Original option pricing paper
- Bollerslev (1986) - GARCH model introduction
- Longstaff-Schwartz (2001) - American option pricing

**Standards:**
- ISO 20022 - Financial services messaging
- FIX Protocol - Electronic trading communication

## License and Contributing

This project is licensed under the MIT License. Contributions are welcome for:
- Additional option types
- New volatility models
- Performance improvements
- Documentation enhancements
- Bug fixes

See the main README for contribution guidelines.
