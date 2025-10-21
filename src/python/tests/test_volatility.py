"""
Unit tests for the ML volatility module
"""

import sys
import numpy as np
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent.parent.parent / 'python'))

from ml_volatility import VolatilityPredictor, GARCHPredictor
from ml_volatility.data_utils import generate_sample_data, preprocess_data


def test_historical_volatility():
    """Test historical volatility calculation"""
    print("Testing historical volatility calculation...")
    
    # Generate sample data
    prices = generate_sample_data(n_days=100, initial_price=100.0, 
                                  mu=0.10, sigma=0.20, seed=42)
    
    # Calculate historical volatility
    vol = VolatilityPredictor.calculate_historical_volatility(prices, window=30)
    
    # Check that volatility is positive and reasonable
    assert vol > 0, "Volatility should be positive"
    assert 0.05 < vol < 1.0, f"Volatility {vol} seems unreasonable"
    
    print(f"  ✓ Historical volatility: {vol*100:.2f}%")
    print("  ✓ Test passed!\n")


def test_garch_model():
    """Test GARCH model fitting and prediction"""
    print("Testing GARCH model...")
    
    # Generate sample data
    prices = generate_sample_data(n_days=252, initial_price=100.0,
                                  mu=0.08, sigma=0.25, seed=42)
    
    # Train GARCH model
    garch = GARCHPredictor(lookback_window=60)
    garch.fit(prices)
    
    assert garch.is_fitted, "Model should be fitted"
    
    # Predict volatility
    vol = garch.predict(prices[-60:])
    
    assert vol > 0, "Predicted volatility should be positive"
    assert 0.05 < vol < 1.0, f"Predicted volatility {vol} seems unreasonable"
    
    print(f"  ✓ GARCH fitted successfully")
    print(f"  ✓ Predicted volatility: {vol*100:.2f}%")
    print("  ✓ Test passed!\n")


def test_data_preprocessing():
    """Test data preprocessing"""
    print("Testing data preprocessing...")
    
    # Create data with outliers
    prices = np.array([100.0, 101.0, 102.0, 150.0, 103.0, 104.0])
    
    # Preprocess
    clean_prices = preprocess_data(prices, remove_outliers=True)
    
    assert len(clean_prices) > 0, "Should have some prices after preprocessing"
    assert len(clean_prices) <= len(prices), "Preprocessing should not add prices"
    
    print(f"  ✓ Preprocessed {len(prices)} -> {len(clean_prices)} prices")
    print("  ✓ Test passed!\n")


def test_returns_calculation():
    """Test returns calculation"""
    print("Testing returns calculation...")
    
    prices = np.array([100.0, 105.0, 102.0, 108.0])
    returns = VolatilityPredictor.calculate_returns(prices)
    
    assert len(returns) == len(prices) - 1, "Should have n-1 returns for n prices"
    
    # First return should be log(105/100)
    expected_first = np.log(105.0 / 100.0)
    assert abs(returns[0] - expected_first) < 1e-10, "Returns calculation incorrect"
    
    print(f"  ✓ Calculated {len(returns)} returns from {len(prices)} prices")
    print("  ✓ Test passed!\n")


def run_all_tests():
    """Run all tests"""
    print("=" * 70)
    print("Running ML Volatility Module Tests")
    print("=" * 70 + "\n")
    
    tests = [
        test_historical_volatility,
        test_garch_model,
        test_data_preprocessing,
        test_returns_calculation,
    ]
    
    passed = 0
    failed = 0
    
    for test_func in tests:
        try:
            test_func()
            passed += 1
        except AssertionError as e:
            print(f"  ✗ Test failed: {e}\n")
            failed += 1
        except Exception as e:
            print(f"  ✗ Test error: {e}\n")
            failed += 1
    
    print("=" * 70)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 70)
    
    return failed == 0


if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)
