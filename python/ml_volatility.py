"""
ML-Based Volatility Forecasting Module
Uses GARCH and machine learning models for volatility prediction
"""

import numpy as np
from typing import List, Tuple, Optional
import warnings
warnings.filterwarnings('ignore')

class GARCHVolatility:
    """
    GARCH(1,1) model for volatility forecasting
    """
    def __init__(self, omega: float = 0.00001, alpha: float = 0.1, beta: float = 0.85):
        self.omega = omega
        self.alpha = alpha
        self.beta = beta
        
    def fit(self, returns: np.ndarray) -> Tuple[float, float, float]:
        """
        Estimate GARCH parameters using maximum likelihood
        Returns: (omega, alpha, beta)
        """
        # Simplified estimation - in production use proper MLE
        variance = np.var(returns)
        self.omega = variance * (1 - self.alpha - self.beta)
        return self.omega, self.alpha, self.beta
    
    def predict(self, returns: np.ndarray, horizon: int = 1) -> float:
        """
        Predict volatility for the next period(s)
        """
        if len(returns) == 0:
            raise ValueError("Returns array is empty")
        
        # Initialize variance
        variance = np.var(returns)
        
        # GARCH recursion
        for r in returns:
            variance = self.omega + self.alpha * r**2 + self.beta * variance
        
        # Multi-step ahead forecast
        for _ in range(horizon - 1):
            variance = self.omega + (self.alpha + self.beta) * variance
        
        # Annualize (assuming daily returns)
        return np.sqrt(variance * 252.0)


class LSTMVolatility:
    """
    LSTM-based volatility forecasting (placeholder for deep learning model)
    In production, this would use TensorFlow/PyTorch
    """
    def __init__(self, lookback: int = 20, hidden_size: int = 50):
        self.lookback = lookback
        self.hidden_size = hidden_size
        self.model = None
        
    def prepare_sequences(self, returns: np.ndarray) -> Tuple[np.ndarray, np.ndarray]:
        """
        Prepare sequences for LSTM training
        """
        X, y = [], []
        for i in range(len(returns) - self.lookback):
            X.append(returns[i:i+self.lookback])
            # Target is realized volatility of next period
            y.append(np.abs(returns[i+self.lookback]))
        return np.array(X), np.array(y)
    
    def fit(self, returns: np.ndarray):
        """
        Train LSTM model (placeholder)
        """
        # In production, this would train a real LSTM model
        pass
    
    def predict(self, returns: np.ndarray) -> float:
        """
        Predict volatility using LSTM
        """
        if len(returns) < self.lookback:
            # Fall back to historical volatility
            return np.std(returns) * np.sqrt(252.0)
        
        # Placeholder: use recent volatility as proxy
        recent_returns = returns[-self.lookback:]
        return np.std(recent_returns) * np.sqrt(252.0)


class EnsembleVolatility:
    """
    Ensemble of multiple volatility models
    """
    def __init__(self):
        self.garch = GARCHVolatility()
        self.lstm = LSTMVolatility()
        self.weights = {'garch': 0.6, 'lstm': 0.4}
        
    def fit(self, returns: np.ndarray):
        """
        Train all models in the ensemble
        """
        self.garch.fit(returns)
        self.lstm.fit(returns)
        
    def predict(self, returns: np.ndarray) -> float:
        """
        Predict volatility using weighted ensemble
        """
        garch_pred = self.garch.predict(returns)
        lstm_pred = self.lstm.predict(returns)
        
        ensemble_pred = (self.weights['garch'] * garch_pred + 
                        self.weights['lstm'] * lstm_pred)
        
        return ensemble_pred


def calculate_historical_volatility(returns: List[float], window: int = 30) -> float:
    """
    Calculate historical volatility
    """
    returns_array = np.array(returns)
    if len(returns_array) < window:
        window = len(returns_array)
    
    recent_returns = returns_array[-window:]
    return np.std(recent_returns) * np.sqrt(252.0)


def calculate_ewma_volatility(returns: List[float], lambda_param: float = 0.94) -> float:
    """
    Calculate EWMA volatility
    """
    returns_array = np.array(returns)
    variance = returns_array[0]**2
    
    for r in returns_array[1:]:
        variance = lambda_param * variance + (1 - lambda_param) * r**2
    
    return np.sqrt(variance * 252.0)


def predict_volatility(returns: List[float], model: str = 'garch') -> float:
    """
    Main function to predict volatility using specified model
    
    Args:
        returns: List of historical returns
        model: 'historical', 'ewma', 'garch', 'lstm', or 'ensemble'
    
    Returns:
        Predicted annualized volatility
    """
    returns_array = np.array(returns)
    
    if model == 'historical':
        return calculate_historical_volatility(returns)
    elif model == 'ewma':
        return calculate_ewma_volatility(returns)
    elif model == 'garch':
        garch = GARCHVolatility()
        garch.fit(returns_array)
        return garch.predict(returns_array)
    elif model == 'lstm':
        lstm = LSTMVolatility()
        lstm.fit(returns_array)
        return lstm.predict(returns_array)
    elif model == 'ensemble':
        ensemble = EnsembleVolatility()
        ensemble.fit(returns_array)
        return ensemble.predict(returns_array)
    else:
        raise ValueError(f"Unknown model: {model}")


if __name__ == "__main__":
    # Example usage
    np.random.seed(42)
    
    # Generate sample returns
    returns = np.random.normal(0, 0.01, 100)
    
    print("Volatility Forecasting Examples:")
    print("-" * 50)
    
    for model in ['historical', 'ewma', 'garch', 'ensemble']:
        vol = predict_volatility(returns.tolist(), model)
        print(f"{model.upper():12s}: {vol:.4f} ({vol*100:.2f}%)")
