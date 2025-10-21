"""
Volatility Prediction Models
Implements GARCH and LSTM-based volatility forecasting
"""

import numpy as np
from typing import List, Optional, Tuple
import warnings


class VolatilityPredictor:
    """Base class for volatility prediction models"""
    
    def __init__(self, lookback_window: int = 30):
        self.lookback_window = lookback_window
        self.is_fitted = False
    
    def fit(self, prices: np.ndarray) -> 'VolatilityPredictor':
        """
        Fit the model to historical price data
        
        Args:
            prices: Array of historical prices
            
        Returns:
            self
        """
        raise NotImplementedError("Subclasses must implement fit()")
    
    def predict(self, prices: Optional[np.ndarray] = None) -> float:
        """
        Predict volatility
        
        Args:
            prices: Optional array of recent prices for prediction
            
        Returns:
            Predicted annualized volatility
        """
        raise NotImplementedError("Subclasses must implement predict()")
    
    @staticmethod
    def calculate_returns(prices: np.ndarray) -> np.ndarray:
        """Calculate log returns from prices"""
        if len(prices) < 2:
            raise ValueError("Need at least 2 prices to calculate returns")
        return np.log(prices[1:] / prices[:-1])
    
    @staticmethod
    def calculate_historical_volatility(prices: np.ndarray, 
                                       window: int = 30,
                                       annualization_factor: int = 252) -> float:
        """
        Calculate historical volatility
        
        Args:
            prices: Array of historical prices
            window: Lookback window
            annualization_factor: Trading days per year (default 252)
            
        Returns:
            Annualized historical volatility
        """
        returns = VolatilityPredictor.calculate_returns(prices)
        if len(returns) < window:
            window = len(returns)
        
        recent_returns = returns[-window:]
        vol = np.std(recent_returns, ddof=1) * np.sqrt(annualization_factor)
        return vol


class GARCHPredictor(VolatilityPredictor):
    """
    GARCH(1,1) volatility predictor
    Simplified implementation for demonstration
    """
    
    def __init__(self, lookback_window: int = 30):
        super().__init__(lookback_window)
        self.omega = 0.000001  # Long-term variance
        self.alpha = 0.1       # ARCH coefficient
        self.beta = 0.85       # GARCH coefficient
        self.current_variance = None
    
    def fit(self, prices: np.ndarray) -> 'GARCHPredictor':
        """
        Fit GARCH model to historical data
        
        Args:
            prices: Array of historical prices
            
        Returns:
            self
        """
        returns = self.calculate_returns(prices)
        
        # Initialize with sample variance
        self.current_variance = np.var(returns, ddof=1)
        
        # Simple parameter estimation using method of moments
        unconditional_var = np.var(returns, ddof=1)
        self.omega = unconditional_var * (1 - self.alpha - self.beta)
        
        # Iteratively update variance
        variances = []
        variance = unconditional_var
        
        for ret in returns:
            variance = self.omega + self.alpha * ret**2 + self.beta * variance
            variances.append(variance)
        
        self.current_variance = variances[-1] if variances else unconditional_var
        self.is_fitted = True
        
        return self
    
    def predict(self, prices: Optional[np.ndarray] = None, 
                horizon: int = 1,
                annualization_factor: int = 252) -> float:
        """
        Predict future volatility using GARCH(1,1)
        
        Args:
            prices: Optional array of recent prices to update model
            horizon: Forecast horizon in days
            annualization_factor: Trading days per year
            
        Returns:
            Predicted annualized volatility
        """
        if not self.is_fitted:
            raise ValueError("Model must be fitted before prediction")
        
        if prices is not None:
            returns = self.calculate_returns(prices)
            # Update variance with recent data
            for ret in returns[-self.lookback_window:]:
                self.current_variance = (self.omega + 
                                        self.alpha * ret**2 + 
                                        self.beta * self.current_variance)
        
        # Forecast variance
        if horizon == 1:
            forecast_variance = self.current_variance
        else:
            # Multi-step forecast
            long_run_var = self.omega / (1 - self.alpha - self.beta)
            forecast_variance = long_run_var + (self.alpha + self.beta)**horizon * (
                self.current_variance - long_run_var
            )
        
        # Annualize and return volatility
        return np.sqrt(forecast_variance * annualization_factor)


class LSTMVolatilityPredictor(VolatilityPredictor):
    """
    LSTM-based volatility predictor
    Requires TensorFlow/Keras (optional dependency)
    """
    
    def __init__(self, lookback_window: int = 30, hidden_size: int = 50):
        super().__init__(lookback_window)
        self.hidden_size = hidden_size
        self.model = None
        self.scaler = None
        
        try:
            from tensorflow import keras
            self.keras_available = True
        except ImportError:
            self.keras_available = False
            warnings.warn("TensorFlow not available. LSTM predictor will not work.")
    
    def _build_model(self, input_shape: Tuple[int, int]):
        """Build LSTM model architecture"""
        if not self.keras_available:
            raise ImportError("TensorFlow/Keras required for LSTM predictor")
        
        from tensorflow import keras
        from tensorflow.keras import layers
        
        model = keras.Sequential([
            layers.LSTM(self.hidden_size, activation='tanh', 
                       input_shape=input_shape, return_sequences=True),
            layers.Dropout(0.2),
            layers.LSTM(self.hidden_size // 2, activation='tanh'),
            layers.Dropout(0.2),
            layers.Dense(1)
        ])
        
        model.compile(optimizer='adam', loss='mse', metrics=['mae'])
        return model
    
    def fit(self, prices: np.ndarray, epochs: int = 50, 
            validation_split: float = 0.2) -> 'LSTMVolatilityPredictor':
        """
        Fit LSTM model to historical data
        
        Args:
            prices: Array of historical prices
            epochs: Training epochs
            validation_split: Validation data proportion
            
        Returns:
            self
        """
        if not self.keras_available:
            raise ImportError("TensorFlow/Keras required for LSTM predictor")
        
        # Calculate rolling volatility as target
        returns = self.calculate_returns(prices)
        
        # Create rolling volatility windows
        rolling_vol = []
        for i in range(self.lookback_window, len(returns)):
            window = returns[i-self.lookback_window:i]
            vol = np.std(window, ddof=1)
            rolling_vol.append(vol)
        
        rolling_vol = np.array(rolling_vol)
        
        # Prepare sequences
        X, y = [], []
        for i in range(self.lookback_window, len(rolling_vol)):
            X.append(rolling_vol[i-self.lookback_window:i])
            y.append(rolling_vol[i])
        
        X = np.array(X).reshape(-1, self.lookback_window, 1)
        y = np.array(y)
        
        # Build and train model
        self.model = self._build_model((self.lookback_window, 1))
        self.model.fit(X, y, epochs=epochs, validation_split=validation_split,
                      verbose=0)
        
        self.is_fitted = True
        return self
    
    def predict(self, prices: Optional[np.ndarray] = None,
                annualization_factor: int = 252) -> float:
        """
        Predict volatility using trained LSTM
        
        Args:
            prices: Array of recent prices
            annualization_factor: Trading days per year
            
        Returns:
            Predicted annualized volatility
        """
        if not self.is_fitted:
            raise ValueError("Model must be fitted before prediction")
        
        if prices is None or len(prices) < self.lookback_window + 1:
            raise ValueError(f"Need at least {self.lookback_window + 1} prices")
        
        returns = self.calculate_returns(prices)
        
        # Calculate rolling volatility
        rolling_vol = []
        for i in range(len(returns) - self.lookback_window + 1):
            window = returns[i:i+self.lookback_window]
            vol = np.std(window, ddof=1)
            rolling_vol.append(vol)
        
        # Use last window for prediction
        X = np.array(rolling_vol[-self.lookback_window:]).reshape(1, self.lookback_window, 1)
        predicted_vol = self.model.predict(X, verbose=0)[0, 0]
        
        # Annualize
        return predicted_vol * np.sqrt(annualization_factor)
