"""
Complete End-to-End Example
Demonstrates the full workflow from data generation to option pricing
"""

import sys
import numpy as np
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent.parent / 'src' / 'python'))

from ml_volatility import VolatilityPredictor, GARCHPredictor
from ml_volatility.data_utils import (
    generate_sample_data, 
    calculate_realized_volatility,
    train_test_split
)


def black_scholes_call(S, K, r, sigma, T):
    """Simple Black-Scholes implementation for comparison"""
    from scipy.stats import norm
    import math
    
    try:
        d1 = (math.log(S/K) + (r + 0.5*sigma**2)*T) / (sigma*math.sqrt(T))
        d2 = d1 - sigma*math.sqrt(T)
        return S*norm.cdf(d1) - K*math.exp(-r*T)*norm.cdf(d2)
    except:
        # Fallback if scipy not available
        return None


def main():
    print("=" * 80)
    print("MC-Pricer: Complete End-to-End Workflow")
    print("=" * 80)
    print()
    
    # Step 1: Generate Historical Data
    print("STEP 1: Generate Historical Price Data")
    print("-" * 80)
    np.random.seed(42)
    n_days = 500
    initial_price = 100.0
    true_mu = 0.10
    true_sigma = 0.25
    
    prices = generate_sample_data(
        n_days=n_days,
        initial_price=initial_price,
        mu=true_mu,
        sigma=true_sigma,
        seed=42
    )
    
    print(f"Generated {n_days} days of synthetic price data")
    print(f"  Initial Price: ${prices[0]:.2f}")
    print(f"  Final Price:   ${prices[-1]:.2f}")
    print(f"  Min Price:     ${np.min(prices):.2f}")
    print(f"  Max Price:     ${np.max(prices):.2f}")
    print(f"  Mean Return:   {np.mean(np.diff(np.log(prices)))*252*100:.2f}% annualized")
    print()
    
    # Step 2: Train-Test Split
    print("STEP 2: Split Data into Training and Test Sets")
    print("-" * 80)
    train_prices, test_prices = train_test_split(prices, test_size=0.2)
    print(f"Training set: {len(train_prices)} days")
    print(f"Test set:     {len(test_prices)} days")
    print()
    
    # Step 3: Calculate Realized Volatility
    print("STEP 3: Calculate Realized Volatility")
    print("-" * 80)
    realized_vols = calculate_realized_volatility(prices, window=30)
    current_realized_vol = realized_vols[-1]
    print(f"30-day realized volatility: {current_realized_vol*100:.2f}%")
    print(f"Average realized volatility: {np.mean(realized_vols)*100:.2f}%")
    print(f"Volatility range: {np.min(realized_vols)*100:.2f}% - {np.max(realized_vols)*100:.2f}%")
    print()
    
    # Step 4: Train Volatility Models
    print("STEP 4: Train ML Volatility Models")
    print("-" * 80)
    
    # Historical Volatility
    hist_vol = VolatilityPredictor.calculate_historical_volatility(
        train_prices, window=30
    )
    print(f"Historical Volatility (30-day): {hist_vol*100:.2f}%")
    
    # GARCH Model
    garch = GARCHPredictor(lookback_window=60)
    garch.fit(train_prices)
    garch_vol = garch.predict(train_prices[-60:])
    print(f"GARCH(1,1) Predicted Volatility: {garch_vol*100:.2f}%")
    print()
    
    # Step 5: Compare Predictions
    print("STEP 5: Compare Volatility Predictions")
    print("-" * 80)
    print(f"True Volatility (simulation):    {true_sigma*100:.2f}%")
    print(f"Historical Volatility:            {hist_vol*100:.2f}%")
    print(f"GARCH Predicted Volatility:       {garch_vol*100:.2f}%")
    print(f"Realized Volatility:              {current_realized_vol*100:.2f}%")
    print()
    
    # Step 6: Option Pricing
    print("STEP 6: Price Options Using Different Volatility Estimates")
    print("-" * 80)
    
    # Option parameters
    S = prices[-1]  # Current spot price
    K = S * 1.05    # 5% out-of-the-money
    r = 0.05        # 5% risk-free rate
    T = 1.0         # 1 year to maturity
    
    print(f"Option Parameters:")
    print(f"  Spot Price (S):       ${S:.2f}")
    print(f"  Strike Price (K):     ${K:.2f}")
    print(f"  Risk-free Rate (r):   {r*100:.2f}%")
    print(f"  Time to Maturity (T): {T:.2f} years")
    print()
    
    # Price with different volatilities
    print(f"Call Option Prices:")
    
    # Try to use scipy for Black-Scholes
    try:
        price_true = black_scholes_call(S, K, r, true_sigma, T)
        print(f"  With True Volatility:      ${price_true:.4f}")
    except:
        print(f"  With True Volatility:      (scipy required)")
    
    try:
        price_hist = black_scholes_call(S, K, r, hist_vol, T)
        print(f"  With Historical Vol:       ${price_hist:.4f}")
    except:
        print(f"  With Historical Vol:       (scipy required)")
    
    try:
        price_garch = black_scholes_call(S, K, r, garch_vol, T)
        print(f"  With GARCH Predicted Vol:  ${price_garch:.4f}")
    except:
        print(f"  With GARCH Predicted Vol:  (scipy required)")
    
    print()
    
    # Step 7: Practical Recommendations
    print("STEP 7: Practical Recommendations")
    print("-" * 80)
    print("For production use:")
    print("  1. Use GARCH or LSTM models for volatility forecasting")
    print("  2. Calibrate models regularly with fresh market data")
    print("  3. Combine multiple volatility estimates (ensemble methods)")
    print("  4. Monitor model performance and retrain as needed")
    print("  5. Use Monte Carlo for path-dependent options")
    print("  6. Implement proper risk management and hedging strategies")
    print()
    
    # Step 8: Integration with C++ Pricer
    print("STEP 8: Integration with C++ Pricer")
    print("-" * 80)
    print("To use ML-predicted volatility with the C++ pricer:")
    print("  1. Run this Python script to get volatility prediction")
    print("  2. Pass volatility to C++ pricer via:")
    print("     - Configuration file")
    print("     - Command-line arguments")
    print("     - Shared memory")
    print("     - Python-C++ bindings (pybind11)")
    print()
    print("Example config file created: volatility_config.txt")
    
    # Write configuration
    config_file = Path(__file__).parent / 'volatility_config.txt'
    with open(config_file, 'w') as f:
        f.write(f"# ML Volatility Configuration\n")
        f.write(f"# Generated by end_to_end_example.py\n\n")
        f.write(f"spot_price={S:.6f}\n")
        f.write(f"strike_price={K:.6f}\n")
        f.write(f"risk_free_rate={r:.6f}\n")
        f.write(f"time_to_maturity={T:.6f}\n")
        f.write(f"volatility_historical={hist_vol:.6f}\n")
        f.write(f"volatility_garch={garch_vol:.6f}\n")
        f.write(f"volatility_realized={current_realized_vol:.6f}\n")
        f.write(f"option_type=call\n")
    
    print()
    print("=" * 80)
    print("End-to-End Workflow Complete!")
    print("=" * 80)


if __name__ == "__main__":
    main()
