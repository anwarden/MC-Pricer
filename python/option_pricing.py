"""
Python bindings for C++ MC-Pricer library
Provides interface between Python ML models and C++ pricing engine
"""

import ctypes
import os
import numpy as np
from typing import List, Dict
from ml_volatility import predict_volatility


class OptionPricer:
    """
    Python interface to C++ option pricing engine
    """
    def __init__(self, library_path: str = None):
        """
        Initialize the pricer with optional path to compiled C++ library
        """
        self.library_path = library_path
        self.lib = None
        
        if library_path and os.path.exists(library_path):
            self._load_library()
    
    def _load_library(self):
        """
        Load the C++ shared library
        """
        try:
            self.lib = ctypes.CDLL(self.library_path)
            # Define function signatures here
        except Exception as e:
            print(f"Warning: Could not load C++ library: {e}")
    
    def black_scholes_price(self, S: float, K: float, r: float, T: float, 
                          sigma: float, option_type: str = 'call') -> float:
        """
        Calculate Black-Scholes price (Python implementation as fallback)
        """
        from scipy.stats import norm
        
        d1 = (np.log(S / K) + (r + 0.5 * sigma**2) * T) / (sigma * np.sqrt(T))
        d2 = d1 - sigma * np.sqrt(T)
        
        if option_type.lower() == 'call':
            price = S * norm.cdf(d1) - K * np.exp(-r * T) * norm.cdf(d2)
        else:
            price = K * np.exp(-r * T) * norm.cdf(-d2) - S * norm.cdf(-d1)
        
        return price
    
    def monte_carlo_price(self, S: float, K: float, r: float, T: float,
                         sigma: float, option_type: str = 'call',
                         num_simulations: int = 100000, num_steps: int = 252) -> Dict:
        """
        Price option using Monte Carlo simulation (Python implementation)
        """
        dt = T / num_steps
        drift = (r - 0.5 * sigma**2) * dt
        diffusion = sigma * np.sqrt(dt)
        
        # Generate random paths
        Z = np.random.standard_normal((num_simulations, num_steps))
        
        # Initialize price paths
        S_paths = np.zeros((num_simulations, num_steps + 1))
        S_paths[:, 0] = S
        
        # Simulate paths
        for t in range(1, num_steps + 1):
            S_paths[:, t] = S_paths[:, t-1] * np.exp(drift + diffusion * Z[:, t-1])
        
        # Calculate payoffs
        final_prices = S_paths[:, -1]
        if option_type.lower() == 'call':
            payoffs = np.maximum(final_prices - K, 0)
        else:
            payoffs = np.maximum(K - final_prices, 0)
        
        # Discount and calculate statistics
        discount = np.exp(-r * T)
        price = discount * np.mean(payoffs)
        se = discount * np.std(payoffs) / np.sqrt(num_simulations)
        
        return {
            'price': price,
            'standard_error': se,
            'confidence_95_lower': price - 1.96 * se,
            'confidence_95_upper': price + 1.96 * se,
            'num_simulations': num_simulations
        }


class VolatilityForecaster:
    """
    Volatility forecasting with ML models
    """
    def __init__(self, model_type: str = 'garch'):
        """
        Initialize forecaster with specified model
        """
        self.model_type = model_type
    
    def forecast(self, returns: List[float]) -> float:
        """
        Forecast volatility using ML model
        """
        return predict_volatility(returns, self.model_type)
    
    def forecast_multiple(self, returns: List[float], 
                         models: List[str] = None) -> Dict[str, float]:
        """
        Forecast using multiple models and return all predictions
        """
        if models is None:
            models = ['historical', 'ewma', 'garch', 'ensemble']
        
        forecasts = {}
        for model in models:
            try:
                forecasts[model] = predict_volatility(returns, model)
            except Exception as e:
                print(f"Warning: Could not compute {model} volatility: {e}")
        
        return forecasts


def generate_sample_returns(n: int = 100, mean: float = 0.0, 
                          vol: float = 0.01, seed: int = 42) -> np.ndarray:
    """
    Generate sample returns for testing
    """
    np.random.seed(seed)
    return np.random.normal(mean, vol, n)


if __name__ == "__main__":
    print("Python ML-Based Option Pricing Module")
    print("=" * 60)
    
    # Example 1: Volatility Forecasting
    print("\n1. Volatility Forecasting")
    print("-" * 60)
    returns = generate_sample_returns(100)
    
    forecaster = VolatilityForecaster('garch')
    forecasts = forecaster.forecast_multiple(returns.tolist())
    
    for model, vol in forecasts.items():
        print(f"{model:12s}: {vol:.4f} ({vol*100:.2f}%)")
    
    # Example 2: Option Pricing
    print("\n2. Option Pricing")
    print("-" * 60)
    
    pricer = OptionPricer()
    
    # Parameters
    S, K = 100.0, 100.0
    r, T = 0.05, 1.0
    sigma = forecasts.get('garch', 0.2)
    
    # Black-Scholes
    bs_price = pricer.black_scholes_price(S, K, r, T, sigma, 'call')
    print(f"Black-Scholes Call: ${bs_price:.4f}")
    
    # Monte Carlo
    mc_result = pricer.monte_carlo_price(S, K, r, T, sigma, 'call', 
                                        num_simulations=10000)
    print(f"Monte Carlo Call:   ${mc_result['price']:.4f} " +
          f"± ${mc_result['standard_error']:.4f}")
    print(f"95% CI: [${mc_result['confidence_95_lower']:.4f}, " +
          f"${mc_result['confidence_95_upper']:.4f}]")
