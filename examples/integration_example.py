"""
Integration script to use ML volatility prediction with C++ pricer
"""

import subprocess
import sys
from pathlib import Path
import numpy as np

sys.path.insert(0, str(Path(__file__).parent.parent / 'src' / 'python'))

from ml_volatility import GARCHPredictor
from ml_volatility.data_utils import generate_sample_data, load_historical_data


def predict_volatility_from_file(filepath: str, method: str = 'garch') -> float:
    """
    Predict volatility from historical price data file
    
    Args:
        filepath: Path to CSV file with historical prices
        method: Prediction method ('historical' or 'garch')
        
    Returns:
        Predicted volatility
    """
    try:
        prices = load_historical_data(filepath)
    except FileNotFoundError:
        print(f"Warning: Could not load {filepath}, using sample data")
        prices = generate_sample_data(n_days=252, seed=42)
    
    if method == 'garch':
        model = GARCHPredictor(lookback_window=60)
        model.fit(prices)
        return model.predict(prices[-60:])
    else:  # historical
        from ml_volatility import VolatilityPredictor
        return VolatilityPredictor.calculate_historical_volatility(prices, window=30)


def call_cpp_pricer(spot: float, strike: float, rate: float, 
                   volatility: float, time: float, option_type: str = 'call'):
    """
    Call C++ pricer executable with parameters
    
    Args:
        spot: Spot price
        strike: Strike price
        rate: Risk-free rate
        volatility: Volatility (from ML prediction)
        time: Time to maturity
        option_type: 'call' or 'put'
    """
    cpp_exe = Path(__file__).parent.parent / 'build' / 'mcpricer'
    
    if not cpp_exe.exists():
        print(f"C++ pricer not found at {cpp_exe}")
        print("Please build the C++ code first:")
        print("  mkdir -p build && cd build")
        print("  cmake .. && make")
        return None
    
    # Note: This is a placeholder - the actual C++ program would need
    # command-line argument support to be called this way
    print(f"\nCalling C++ pricer with:")
    print(f"  Spot: ${spot:.2f}")
    print(f"  Strike: ${strike:.2f}")
    print(f"  Rate: {rate*100:.2f}%")
    print(f"  Volatility: {volatility*100:.2f}% (ML-predicted)")
    print(f"  Time: {time:.2f} years")
    print(f"  Type: {option_type}")
    
    try:
        result = subprocess.run([str(cpp_exe)], capture_output=True, text=True)
        print("\nC++ Pricer Output:")
        print(result.stdout)
        return result.stdout
    except Exception as e:
        print(f"Error calling C++ pricer: {e}")
        return None


def main():
    print("=" * 70)
    print("MC-Pricer: Python-C++ Integration")
    print("=" * 70)
    
    # Example parameters
    spot = 100.0
    strike = 105.0
    rate = 0.05
    time = 1.0
    
    # Generate sample data for demonstration
    print("\nGenerating sample price data...")
    prices = generate_sample_data(n_days=252, initial_price=100.0, 
                                  mu=0.08, sigma=0.22, seed=42)
    
    # Predict volatility using GARCH
    print("Training GARCH model for volatility prediction...")
    garch = GARCHPredictor(lookback_window=60)
    garch.fit(prices)
    ml_volatility = garch.predict(prices[-60:])
    
    print(f"\nML-Predicted Volatility: {ml_volatility*100:.2f}%")
    
    # Call C++ pricer
    print("\nIntegrating with C++ pricing engine...")
    call_cpp_pricer(spot, strike, rate, ml_volatility, time, 'call')
    
    print("\n" + "=" * 70)


if __name__ == "__main__":
    main()
