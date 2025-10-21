# MC-Pricer Quick Start Guide

## Installation

### Prerequisites
```bash
# C++ compiler (GCC 7+, Clang 5+, or MSVC 2017+)
# CMake 3.12+
# Python 3.7+
```

### Build C++ Components
```bash
git clone https://github.com/anwarden/MC-Pricer.git
cd MC-Pricer
mkdir build && cd build
cmake ..
make
```

### Install Python Dependencies
```bash
pip install numpy
# Optional for LSTM:
pip install tensorflow
```

## Quick Examples

### Example 1: Price a European Call Option (C++)
```bash
./build/mcpricer
```

Output:
```
=== Black-Scholes Analytical Pricing ===
Option Price: $14.1598
Delta:   0.5719
Gamma:   0.0110
Vega:    0.3924
```

### Example 2: Predict Volatility (Python)
```python
from ml_volatility import GARCHPredictor
from ml_volatility.data_utils import generate_sample_data

# Generate sample prices
prices = generate_sample_data(n_days=252)

# Train GARCH model
garch = GARCHPredictor(lookback_window=60)
garch.fit(prices)

# Predict volatility
vol = garch.predict(prices[-60:])
print(f"Predicted Volatility: {vol*100:.2f}%")
```

### Example 3: Complete Workflow
```bash
# Run end-to-end example
python examples/end_to_end_example.py

# Run ML demo
python examples/ml_volatility_demo.py

# Run integration example
python examples/integration_example.py
```

## Key Commands

### Building
```bash
# Release build (optimized)
cmake -DCMAKE_BUILD_TYPE=Release ..
make

# Debug build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

# Clean and rebuild
make clean && make
```

### Testing
```bash
# Run C++ tests
./build/test_pricer

# Run Python tests
python src/python/tests/test_volatility.py
```

### Running Examples
```bash
# C++ pricer
./build/mcpricer

# Python examples
python examples/ml_volatility_demo.py
python examples/integration_example.py
python examples/end_to_end_example.py
```

## Common Use Cases

### Use Case 1: Price Options with Historical Volatility

**Python:**
```python
from ml_volatility import VolatilityPredictor

# Calculate historical volatility
prices = [100, 102, 101, 103, 105, 104, 106]
vol = VolatilityPredictor.calculate_historical_volatility(prices, window=5)
print(f"Historical Vol: {vol*100:.2f}%")
```

**C++:**
```cpp
std::vector<double> prices = {100, 102, 101, 103, 105, 104, 106};
HistoricalVolatility histVol(5, 252);
double vol = histVol.predictVolatility(prices);

EuropeanOption option(105.0, 105.0, 0.05, vol, 1.0, "call");
double price = option.calculatePrice();
```

### Use Case 2: Compare Pricing Methods

```cpp
EuropeanOption option(100, 105, 0.05, 0.25, 1.0, "call");

// Black-Scholes
double bsPrice = option.calculatePrice();

// Monte Carlo
MonteCarloEngine mc(100000, 252);
auto [mcPrice, mcError] = mc.priceEuropeanOption(option);

// Compare
double diff = std::abs(bsPrice - mcPrice);
```

### Use Case 3: Calculate Greeks

```cpp
EuropeanOption option(100, 100, 0.05, 0.20, 1.0, "call");

std::cout << "Price: " << option.calculatePrice() << std::endl;
std::cout << "Delta: " << option.calculateDelta() << std::endl;
std::cout << "Gamma: " << option.calculateGamma() << std::endl;
std::cout << "Vega:  " << option.calculateVega() << std::endl;
std::cout << "Theta: " << option.calculateTheta() << std::endl;
std::cout << "Rho:   " << option.calculateRho() << std::endl;
```

### Use Case 4: Forecast Volatility with GARCH

```python
from ml_volatility import GARCHPredictor

# Historical data (500 days)
prices = [...]  # Your price data

# Split into train/test
train_data = prices[:400]
test_data = prices[400:]

# Train GARCH
garch = GARCHPredictor(lookback_window=60)
garch.fit(train_data)

# Predict for test period
predictions = []
for i in range(len(test_data)):
    vol = garch.predict(train_data[-60:])
    predictions.append(vol)
    train_data = train_data[1:] + [test_data[i]]
```

### Use Case 5: Batch Option Pricing

```cpp
std::vector<double> strikes = {95, 100, 105, 110, 115};
double S = 100, r = 0.05, sigma = 0.20, T = 1.0;

for (double K : strikes) {
    EuropeanOption call(S, K, r, sigma, T, "call");
    EuropeanOption put(S, K, r, sigma, T, "put");
    
    std::cout << "K=" << K 
              << " Call=" << call.calculatePrice()
              << " Put=" << put.calculatePrice() << std::endl;
}
```

## Configuration Options

### Monte Carlo Engine
```cpp
MonteCarloEngine mc(
    100000,  // numSimulations - more = more accurate
    252,     // numSteps - time discretization
    42       // seed - for reproducibility
);
```

### GARCH Model
```python
garch = GARCHPredictor(
    lookback_window=60  # Days of history for prediction
)
```

### Historical Volatility
```python
vol = VolatilityPredictor.calculate_historical_volatility(
    prices,
    window=30,              # Lookback window
    annualization_factor=252  # Trading days per year
)
```

## Troubleshooting

### "Command not found: cmake"
```bash
# Ubuntu/Debian
sudo apt-get install cmake

# macOS
brew install cmake
```

### "No module named 'numpy'"
```bash
pip install numpy
```

### "Permission denied: ./mcpricer"
```bash
chmod +x build/mcpricer
```

### Monte Carlo price differs from Black-Scholes
- Increase number of simulations
- This is normal - MC has statistical error
- Check the error estimate

### GARCH gives unrealistic volatility
- Ensure enough historical data (at least 60 days)
- Check for data quality issues
- Try adjusting lookback window

## Performance Tips

### C++ Optimization
```bash
# Use optimized build
cmake -DCMAKE_BUILD_TYPE=Release ..
make

# Profile the code
./build/mcpricer
time ./build/mcpricer
```

### Python Optimization
```python
# Use NumPy operations
import numpy as np
returns = np.log(prices[1:] / prices[:-1])  # Vectorized

# Batch operations
vols = [garch.predict(prices[i:i+60]) for i in range(len(prices)-60)]
```

## File Structure Reference

```
MC-Pricer/
├── src/cpp/
│   ├── include/          # Header files
│   │   ├── Option.h
│   │   ├── EuropeanOption.h
│   │   ├── MonteCarloEngine.h
│   │   └── VolatilityInterface.h
│   ├── src/              # Implementation
│   │   ├── Option.cpp
│   │   ├── EuropeanOption.cpp
│   │   ├── MonteCarloEngine.cpp
│   │   ├── VolatilityInterface.cpp
│   │   └── main.cpp
│   └── tests/
│       └── test_pricer.cpp
│
├── src/python/
│   ├── ml_volatility/
│   │   ├── __init__.py
│   │   ├── volatility_predictor.py
│   │   └── data_utils.py
│   └── tests/
│       └── test_volatility.py
│
├── examples/
│   ├── ml_volatility_demo.py
│   ├── integration_example.py
│   └── end_to_end_example.py
│
├── CMakeLists.txt
├── requirements.txt
├── README.md
├── TECHNICAL_GUIDE.md
└── QUICKSTART.md
```

## Next Steps

1. **Read the Technical Guide** - TECHNICAL_GUIDE.md for in-depth details
2. **Run Examples** - Try all examples in the examples/ directory
3. **Modify Parameters** - Experiment with different option parameters
4. **Add Features** - Extend with American options, dividends, etc.
5. **Optimize** - Profile and optimize for your use case

## Getting Help

- Check TECHNICAL_GUIDE.md for detailed explanations
- Review examples/ directory for code patterns
- Run tests to verify your setup
- See README.md for overview and installation

## Quick Reference Card

| Task | Command |
|------|---------|
| Build C++ | `mkdir build && cd build && cmake .. && make` |
| Run pricer | `./build/mcpricer` |
| Run tests | `./build/test_pricer` |
| Install Python deps | `pip install -r requirements.txt` |
| Run Python demo | `python examples/ml_volatility_demo.py` |
| Clean build | `cd build && make clean` |

## Important Functions

### C++ API
```cpp
// Create option
EuropeanOption(S, K, r, sigma, T, type);

// Price and Greeks
calculatePrice()
calculateDelta()
calculateGamma()
calculateVega()
calculateTheta()
calculateRho()

// Monte Carlo
MonteCarloEngine mc(simulations, steps, seed);
mc.priceEuropeanOption(option);
```

### Python API
```python
# Volatility prediction
GARCHPredictor(lookback_window)
predictor.fit(prices)
predictor.predict(prices)

# Data utilities
generate_sample_data(n_days, initial_price, mu, sigma, seed)
load_historical_data(filepath, price_column)
preprocess_data(prices, remove_outliers)
calculate_realized_volatility(prices, window)
```

---

**Happy Pricing!** 🚀📈
