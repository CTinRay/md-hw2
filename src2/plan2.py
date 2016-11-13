import numpy as np


class Plan2:    

    def __init__(self, features):
        self.features = features
        self.weights = np.empty(features.shape[1])
        self.weights.fill(1)

        
    def iter(self, learning_rate=0.01):
        scores = self.weights * self.features        
        median = np.median(self.scores)
        upper = self.features[np.where(scores > median)]
        lower = self.features[np.where(scores < median)]
        gradient = np.sum(upper, axis=0) - np.sum(lower, axis=0)
        self.weights -= gradient * learning_rate

        
    def predict():
        scores = weights * features
        median = np.median(self.scores)
        result = np.zeros(features.shape[0])
        result[np.where(scores > median)] = 1
        return result

    
