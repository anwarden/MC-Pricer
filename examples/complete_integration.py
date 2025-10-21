"""
Complete Integration Example: C++ Pricing Engine + Python ML
Demonstrates the full workflow of the MC-Pricer system
"""

import numpy as np
import os
import sys

# Add python directory to path
script_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(script_dir)
sys.path.insert(0, os.path.join(parent_dir, 'python'))

from option_pricing import OptionPricer, VolatilityForecaster
from ml_volatility import predict_volatility

def main():
    print("=" * 70)
    print("MC-Pricer: Complete Integration Example")
    print("=" * 70)
    
    # Step 1: Generate historical market data
    print("\n1. Simulating Historical Market Data")
    print("-" * 70)
    
    np.random.seed(42)
    num_days = 252  # One year of trading days
    
    # Simulate returns with regime change
    returns_low_vol = np.random.normal(0.0005, 0.01, 126)
    returns_high_vol = np.random.normal(0.0005, 0.025, 126)
    returns = np.concatenate([returns_low_vol, returns_high_vol])
    
    print(f"Generated {len(returns)} daily returns")
    print(f"Recent volatility regime: {'HIGH' if np.std(returns[-30:]) > 0.015 else 'LOW'}")
    
    # Step 2: Forecast Volatility Using Multiple Models
    print("\n2. Volatility Forecasting with ML Models")
    print("-" * 70)
    
    forecaster = VolatilityForecaster()
    vol_forecasts = forecaster.forecast_multiple(returns.tolist())
    
    print(f"{'Model':<15} {'Volatility':<12} {'Annual %'}")
    print("-" * 70)
    for model, vol in vol_forecasts.items():
        print(f"{model:<15} {vol:>10.4f}   {vol*100:>6.2f}%")
    
    # Use ensemble for pricing
    selected_vol = vol_forecasts.get('ensemble', 0.15)
    print(f"\nSelected volatility (Ensemble): {selected_vol:.4f}")
    
    # Step 3: Price Options Using Forecasted Volatility
    print("\n3. Option Pricing with Forecasted Volatility")
    print("-" * 70)
    
    # Market parameters
    S0 = 100.0      # Current stock price
    K_otm = 105.0   # Out-of-the-money call
    K_atm = 100.0   # At-the-money call
    K_itm = 95.0    # In-the-money call
    r = 0.03        # 3% risk-free rate
    T = 0.25        # 3 months to expiry
    
    pricer = OptionPricer()
    
    strikes = [K_itm, K_atm, K_otm]
    labels = ['ITM (K=95)', 'ATM (K=100)', 'OTM (K=105)']
    
    print(f"\nMarket: S=${S0}, r={r*100:.1f}%, T={T*12:.1f}mo, σ={selected_vol*100:.1f}%")
    print("\nCall Option Prices:")
    print(f"{'Strike':<12} {'BS Price':<12} {'MC Price':<12} {'MC Std Err'}")
    print("-" * 70)
    
    for strike, label in zip(strikes, labels):
        # Black-Scholes
        bs_price = pricer.black_scholes_price(
            S=S0, K=strike, r=r, T=T, sigma=selected_vol, option_type='call'
        )
        
        # Monte Carlo (fewer simulations for speed)
        mc_result = pricer.monte_carlo_price(
            S=S0, K=strike, r=r, T=T, sigma=selected_vol, 
            option_type='call', num_simulations=50000
        )
        
        print(f"{label:<12} ${bs_price:>9.4f}   ${mc_result['price']:>9.4f}   "
              f"${mc_result['standard_error']:>9.4f}")
    
    # Step 4: Risk Analysis (Greeks)
    print("\n4. Risk Metrics (Greeks) for ATM Call")
    print("-" * 70)
    
    # Calculate Greeks using finite differences
    delta_bs = pricer.black_scholes_price(S0+0.01, K_atm, r, T, selected_vol, 'call') - \
               pricer.black_scholes_price(S0-0.01, K_atm, r, T, selected_vol, 'call')
    delta_bs /= 0.02
    
    vega_bs = pricer.black_scholes_price(S0, K_atm, r, T, selected_vol+0.001, 'call') - \
              pricer.black_scholes_price(S0, K_atm, r, T, selected_vol-0.001, 'call')
    vega_bs /= 0.002
    
    print(f"Delta:    {delta_bs:.4f}  ($ change per $1 move in underlying)")
    print(f"Vega:     {vega_bs:.4f}  ($ change per 1% vol move)")
    
    # Step 5: Scenario Analysis
    print("\n5. Scenario Analysis: What if volatility was underestimated?")
    print("-" * 70)
    
    true_vol = selected_vol * 1.5  # 50% higher volatility
    
    print(f"Forecasted vol: {selected_vol*100:.2f}%")
    print(f"True vol:       {true_vol*100:.2f}%")
    
    price_forecasted = pricer.black_scholes_price(S0, K_atm, r, T, selected_vol, 'call')
    price_true = pricer.black_scholes_price(S0, K_atm, r, T, true_vol, 'call')
    
    mispricing = price_true - price_forecasted
    mispricing_pct = (mispricing / price_forecasted) * 100
    
    print(f"\nATM Call price (forecasted vol): ${price_forecasted:.4f}")
    print(f"ATM Call price (true vol):       ${price_true:.4f}")
    print(f"Mispricing:                      ${mispricing:.4f} ({mispricing_pct:+.2f}%)")
    
    # Step 6: Summary
    print("\n" + "=" * 70)
    print("Summary: Complete ML-Enhanced Option Pricing Workflow")
    print("=" * 70)
    print("\n✓ Historical data analysis")
    print("✓ Multi-model volatility forecasting (GARCH, EWMA, Ensemble)")
    print("✓ Black-Scholes analytical pricing")
    print("✓ Monte Carlo validation")
    print("✓ Risk metrics calculation")
    print("✓ Scenario analysis")
    print("\nThis demonstrates the integration of classical quantitative finance")
    print("with modern machine learning for production trading applications.")
    print("=" * 70)

if __name__ == "__main__":
    main()
