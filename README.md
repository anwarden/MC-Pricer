# AI-Enhanced Monte Carlo Option Pricer

Quantitative Finance | C++ | Machine Learning

This repository contains a production-ready skeleton for an option pricing engine that combines analytic Black–Scholes formulas with Monte Carlo simulation and an extensible ML-based volatility forecasting module.

Goals
- Object-oriented C++ architecture focused on performance and extensibility
- Monte Carlo pricer for path simulation and Greeks
- Black–Scholes analytic engine for sanity checks and control variates
- Python ML module scaffold for volatility prediction (train/predict)
- CI to build C++ and run basic checks

Project layout
- `CMakeLists.txt` — top-level CMake
- `include/` — public C++ headers (Option, BlackScholes, MonteCarloPricer, VolatilityModel)
- `src/` — C++ implementation and demo CLI
- `python/vol_model/` — Python module scaffold for ML volatility forecasting
- `.github/workflows/ci.yml` — CI to build and run tests
- `tests/` — basic runtime check program

Quick start (Linux/macOS)
1. Install dependencies:
   - C++ toolchain with CMake (>= 3.10), make/ninja
   - Python 3.8+ (for ML module)
2. Build:
   mkdir build && cd build
   cmake ..
   cmake --build .
3. Run demo:
   ./demo/pricer_demo
4. Python ML (virtualenv recommended):
   cd python
   pip install -r requirements.txt
   python vol_model/model.py --help

Design notes
- C++ code uses simple interfaces to allow replacing volatility sources (e.g., plug in a wrapper calling the Python model via IPC or embed Python).
- Monte Carlo pricer is templated to allow switching RNG / parallelization strategy later.
- Python ML module uses scikit-learn as a placeholder. It provides scripts to train and save a model and to predict volatility.

Next steps
- Implement a production-grade volatility connector between C++ and Python (pybind11, REST API, or gRPC).
- Add real-market data ingest and feature engineering for volatility forecasting.
- Extend Monte Carlo for variance reduction techniques and path-dependent payoffs.
- Add unit tests and benchmarks (Google Benchmark) for perf profiling.

License
- MIT
