import numpy as np
import argparse

class Plan2:    

    def __init__(self, features):
        self.features = features
        self.weights = np.empty(features.shape[1])
        self.weights.fill(1)

        
    def iteration(self, learning_rate=0.01, verbose=False):
        scores = np.sum(self.weights * self.features, axis=1)
        median = np.median(scores)
        upper = np.where(scores > median)
        lower = np.where(scores < median)
        feature_upper = np.sum(self.features[upper], axis=0)
        feature_lower = np.sum(self.features[lower], axis=0)
        score_upper = np.sum(scores[upper])
        score_lower = np.sum(scores[lower])
        gradient = (feature_upper * score_lower - feature_lower * score_upper) / score_lower ** 2

        if verbose:
            print('|gradient| =', np.linalg.norm(gradient))
            print('score_upper - scopre_lower =', score_upper - score_lower)
            
        self.weights += gradient * learning_rate
        return gradient

    
    def fit(self, converge=0.001, learning_rate=0.01, verbose=False):
        gradient = self.iteration(learning_rate, verbose)
        while np.linalg.norm(gradient) > converge:
            gradient = self.iteration(learning_rate, verbose)

        
    def predict(self):
        scores = np.sum(self.weights * self.features, axis=1)
        median = np.median(scores)
        result = np.zeros(self.features.shape[0])
        result[np.where(scores > median)] = 1
        return result


def load_feature(filename):
    f = open(filename)
    fstl = f.readline()
    features = []
    for l in f:
        features += [list(map(float, l.strip().split(' ')))]
    return np.array(features)
    

def main():
    parser = argparse.ArgumentParser(description='===== PLAN II =====')
    parser.add_argument('features', type=str, help='features')
    parser.add_argument('output', type=str, help='output')
    parser.add_argument('--converge', type=float, help='converge, default = 0.001')
    parser.add_argument('--verbose', type=bool, help='verbose, default = False')    
    parser.add_argument('--learning_rate', type=bool, help='learning rate, default = 0.01')    
    
    args = parser.parse_args()

    features = load_feature(args.features)
    plan2 = Plan2(features)
    plan2.fit(learning_rate=args.learning_rate,
              verbose=args.verbose)

    f = open(args.output, 'w')
    ys = plan2.predict()
    for y in ys:
        f.write(str(int(y)) + '\n')
    

if __name__ == '__main__':
    main()
