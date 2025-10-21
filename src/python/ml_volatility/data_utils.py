"""
Data utilities for volatility prediction
"""

import numpy as np
from typing import List, Tuple, Optional
import csv
from pathlib import Path


def load_historical_data(filepath: str, price_column: str = 'Close') -> np.ndarray:
    """
    Load historical price data from CSV file
    
    Args:
        filepath: Path to CSV file
        price_column: Name of the price column
        
    Returns:
        Array of prices
    """
    prices = []
    
    try:
        with open(filepath, 'r') as f:
            reader = csv.DictReader(f)
            for row in reader:
                if price_column in row:
                    try:
                        price = float(row[price_column])
                        prices.append(price)
                    except (ValueError, TypeError):
                        continue
    except FileNotFoundError:
        raise FileNotFoundError(f"Data file not found: {filepath}")
    
    if not prices:
        raise ValueError(f"No valid prices found in {filepath}")
    
    return np.array(prices)


def preprocess_data(prices: np.ndarray, 
                   remove_outliers: bool = True,
                   outlier_threshold: float = 3.0) -> np.ndarray:
    """
    Preprocess price data
    
    Args:
        prices: Array of prices
        remove_outliers: Whether to remove outliers
        outlier_threshold: Z-score threshold for outlier removal
        
    Returns:
        Preprocessed prices
    """
    if len(prices) == 0:
        raise ValueError("Empty price array")
    
    # Remove NaN and negative values
    prices = prices[~np.isnan(prices)]
    prices = prices[prices > 0]
    
    if len(prices) == 0:
        raise ValueError("No valid prices after filtering")
    
    if remove_outliers and len(prices) > 3:
        # Calculate returns
        returns = np.log(prices[1:] / prices[:-1])
        
        # Identify outliers using z-score
        mean_ret = np.mean(returns)
        std_ret = np.std(returns, ddof=1)
        
        if std_ret > 0:
            z_scores = np.abs((returns - mean_ret) / std_ret)
            # Keep prices where return is not an outlier
            valid_indices = np.concatenate([[True], z_scores < outlier_threshold])
            prices = prices[valid_indices]
    
    return prices


def generate_sample_data(n_days: int = 252,
                         initial_price: float = 100.0,
                         mu: float = 0.05,
                         sigma: float = 0.2,
                         seed: Optional[int] = None) -> np.ndarray:
    """
    Generate synthetic price data using geometric Brownian motion
    
    Args:
        n_days: Number of days to simulate
        initial_price: Starting price
        mu: Expected annual return
        sigma: Annual volatility
        seed: Random seed for reproducibility
        
    Returns:
        Array of simulated prices
    """
    if seed is not None:
        np.random.seed(seed)
    
    dt = 1.0 / 252  # Daily time step
    prices = np.zeros(n_days)
    prices[0] = initial_price
    
    for i in range(1, n_days):
        z = np.random.standard_normal()
        prices[i] = prices[i-1] * np.exp((mu - 0.5 * sigma**2) * dt + 
                                         sigma * np.sqrt(dt) * z)
    
    return prices


def calculate_realized_volatility(prices: np.ndarray,
                                  window: int = 30,
                                  annualization_factor: int = 252) -> np.ndarray:
    """
    Calculate rolling realized volatility
    
    Args:
        prices: Array of prices
        window: Rolling window size
        annualization_factor: Trading days per year
        
    Returns:
        Array of realized volatilities
    """
    returns = np.log(prices[1:] / prices[:-1])
    
    realized_vols = []
    for i in range(window, len(returns) + 1):
        window_returns = returns[i-window:i]
        vol = np.std(window_returns, ddof=1) * np.sqrt(annualization_factor)
        realized_vols.append(vol)
    
    return np.array(realized_vols)


def train_test_split(prices: np.ndarray, 
                     test_size: float = 0.2) -> Tuple[np.ndarray, np.ndarray]:
    """
    Split price data into training and test sets
    
    Args:
        prices: Array of prices
        test_size: Proportion of data for testing
        
    Returns:
        Tuple of (train_prices, test_prices)
    """
    if not 0 < test_size < 1:
        raise ValueError("test_size must be between 0 and 1")
    
    split_idx = int(len(prices) * (1 - test_size))
    return prices[:split_idx], prices[split_idx:]
