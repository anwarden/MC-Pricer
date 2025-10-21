"""
Python-C++ Integration Example
Demonstrates how to use ML-based volatility prediction with C++ option pricing
"""

import sys
import numpy as np
from pathlib import Path

# Add the ml_volatility module to path
sys.path.insert(0, str(Path(__file__).parent.parent / 'src' / 'python'))

from ml_volatility import VolatilityPredictor, GARCHPredictor
from ml_volatility.data_utils import generate_sample_data


def format_price(price: float) -> str:
    """Format price for display"""
    return f"${price:.4f}"


def format_percent(value: float) -> str:
    """Format percentage for display"""
    return f"{value * 100:.2f}%"


def main():
    print("=" * 70)
    print("MC-Pricer: ML-Based Volatility Prediction Demo")
    print("=" * 70)
    
    # Generate sample data
    print("\n1. Generating sample price data...")
    prices = generate_sample_data(n_days=500, initial_price=100.0, 
                                  mu=0.10, sigma=0.25, seed=42)
    print(f"   Generated {len(prices)} days of price data")
    print(f"   Initial price: {format_price(prices[0])}")
    print(f"   Final price: {format_price(prices[-1])}")
    print(f"   Price range: {format_price(np.min(prices))} - {format_price(np.max(prices))}")
    
    # Calculate historical volatility
    print("\n2. Calculating historical volatility...")
    hist_vol = VolatilityPredictor.calculate_historical_volatility(prices, window=30)
    print(f"   Historical volatility (30-day): {format_percent(hist_vol)}")
    
    # GARCH prediction
    print("\n3. Training GARCH(1,1) model...")
    garch_model = GARCHPredictor(lookback_window=60)
    garch_model.fit(prices[:400])  # Train on first 400 days
    print("   GARCH model fitted successfully")
    
    # Predict on recent data
    garch_vol = garch_model.predict(prices[-60:])
    print(f"   GARCH predicted volatility: {format_percent(garch_vol)}")
    
    # Compare predictions
    print("\n4. Volatility Comparison:")
    print(f"   Historical (30-day):  {format_percent(hist_vol)}")
    print(f"   GARCH(1,1):          {format_percent(garch_vol)}")
    
    # Option pricing parameters
    print("\n5. Option Pricing Parameters:")
    S = prices[-1]  # Current spot price
    K = S * 1.05    # Strike at 105% of spot
    r = 0.05        # 5% risk-free rate
    T = 1.0         # 1 year to maturity
    
    print(f"   Spot Price (S):       {format_price(S)}")
    print(f"   Strike Price (K):     {format_price(K)}")
    print(f"   Risk-free Rate (r):   {format_percent(r)}")
    print(f"   Time to Maturity (T): {T:.2f} years")
    
    # Demonstrate integration concept
    print("\n6. Integration with C++ Pricer:")
    print("   The predicted volatility can be passed to the C++ pricing engine")
    print("   using one of these methods:")
    print("   - Write volatility to a config file that C++ reads")
    print("   - Use Python subprocess to call C++ executable with parameters")
    print("   - Use pybind11 for direct C++-Python binding (advanced)")
    
    # Example: Write config file
    config_file = Path(__file__).parent.parent / 'examples' / 'volatility_config.txt'
    config_file.parent.mkdir(exist_ok=True)
    with open(config_file, 'w') as f:
        f.write(f"# ML-Predicted Volatility Configuration\n")
        f.write(f"volatility={garch_vol:.6f}\n")
        f.write(f"prediction_method=GARCH\n")
        f.write(f"spot_price={S:.2f}\n")
        f.write(f"strike_price={K:.2f}\n")
    
    print(f"\n   Configuration written to: {config_file}")
    
    # Optional: LSTM example (if TensorFlow available)
    print("\n7. Advanced ML Models:")
    try:
        from ml_volatility.volatility_predictor import LSTMVolatilityPredictor
        
        print("   LSTM-based prediction is available (TensorFlow detected)")
        print("   To train LSTM model:")
        print("   >>> lstm_model = LSTMVolatilityPredictor(lookback_window=30)")
        print("   >>> lstm_model.fit(prices, epochs=50)")
        print("   >>> lstm_vol = lstm_model.predict(prices)")
    except ImportError:
        print("   LSTM requires TensorFlow (optional dependency)")
        print("   Install with: pip install tensorflow")
    
    print("\n" + "=" * 70)
    print("Demo completed successfully!")
    print("=" * 70)


if __name__ == "__main__":
    main()
