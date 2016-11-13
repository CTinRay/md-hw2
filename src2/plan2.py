import numpy as np


class Plan2:    

    def __init__(self, features):
        self.features = features
        self.weights = np.empty(features.shape[1])
        self.weights.fill(1)

        
    def iter(self, learning_rate=0.01):
        scores = self.weights * self.features        
        median = np.median(self.scores)
        feature_upper = np.sum(self.features[np.where(scores > median)], axis=0)
        feature_lower = np.sum(self.features[np.where(scores < median)], axis=0)
        score_upper = np.sum(scores[np.where(scores > median)])
        score_lower = np.sum(scores[np.where(scores < median)])
        gradient = (feature_upper * score_lower - feature_lower * score_upper) / score_lower ** 2
        self.weights -= gradient * learning_rate

        
    def predict():
        scores = weights * features
        median = np.median(self.scores)
        result = np.zeros(features.shape[0])
        result[np.where(scores > median)] = 1
        return result

    
