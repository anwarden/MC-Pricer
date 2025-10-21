"""
MC-Pricer ML Volatility Module
Machine Learning-based volatility forecasting for option pricing
"""

__version__ = "1.0.0"
__author__ = "MC-Pricer Team"

from .volatility_predictor import VolatilityPredictor, GARCHPredictor
from .data_utils import load_historical_data, preprocess_data

__all__ = [
    'VolatilityPredictor',
    'GARCHPredictor',
    'load_historical_data',
    'preprocess_data'
]
