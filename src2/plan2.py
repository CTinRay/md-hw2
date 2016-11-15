import numpy as np
import argparse

class Plan2:    

    def __init__(self, features):
        self.features = features
        self.weights = np.empty(features.shape[1])
        self.weights.fill(1)
        self.prev_gradient = np.zeros(features.shape[1])
        
    def iteration(self, learning_rate=0.01, verbose=False, answer=None):
        scores = np.sum(self.weights * self.features, axis=1)
        median = np.median(scores)
        upper = np.where(scores > median)[0][-4000:]
        lower = np.where(scores < median)[0][:4000]

        # upper = np.random.choice(upper, 20000)
        # lower = np.random.choice(lower, 20000)

        feature_upper = np.sum(self.features[upper], axis=0)
        feature_lower = np.sum(self.features[lower], axis=0)        
        score_upper = np.sum(scores[upper])
        score_lower = np.sum(scores[lower])
        weights_sum = np.sum(self.weights)
        gradient = ((feature_upper * weights_sum - score_upper) -
                    (feature_lower * weights_sum - score_lower))/ weights_sum**2
        gradient = (gradient + self.prev_gradient) / 2
        self.prev_gradient = gradient
        
        if verbose is True:
            print('|gradient| =', np.linalg.norm(gradient))
            print('score_upper - scopre_lower =', (score_upper - score_lower) / sum(self.weights),
                  'std_upper:', np.std(scores[upper]),
                  'std_lower:', np.std(scores[lower]),
                  'max_upper:', np.max(scores[upper]),
                  'median:', np.median(scores),
                  'min_lower:', np.min(scores[lower]) )
            print('weights:', self.weights)
            # print('gradient', gradient)
            if answer is not None:
                ys = self.predict()
                print('accuracy:', np.sum(answer == ys) / ys.shape[0])
            
        self.weights += gradient * learning_rate
        return gradient

    
    def fit(self, converge=0.001, learning_rate=0.01, verbose=False, answer=None):
        gradient = self.iteration(learning_rate, verbose)
        while np.linalg.norm(gradient) > converge:
            gradient = self.iteration(learning_rate, verbose, answer)

        
    def predict(self):
        scores = np.sum(self.weights * self.features, axis=1)
        median = np.median(scores)
        result = np.zeros(self.features.shape[0],dtype=int)
        result[np.where(scores > median)] = 1
        return result


def load_feature(filename):
    f = open(filename)
    features = []
    for l in f:
        features += [list(map(float, l.strip().split(' ')))]
    return np.array(features)
    

def main():
    parser = argparse.ArgumentParser(description='===== PLAN II =====')
    parser.add_argument('features', type=str, help='features')
    parser.add_argument('output', type=str, help='output')
    parser.add_argument('--converge', type=float, help='converge, default = 0.001')
    parser.add_argument('--verbose', type=bool, help='verbose, default = False', default=False)    
    parser.add_argument('--learning_rate', type=float, help='learning rate, default = 0.01', default=0.001)    
    parser.add_argument('--answer', type=str, help='answer')    
    
    args = parser.parse_args()

    answer = None
    if args.answer is not None:
        f = open(args.answer)
        answer = np.array(list(map(int, f.read().strip().split('\n'))), dtype=int)
        
    features = load_feature(args.features) #[:,[0, 1, 3, 4, 6]]
    features_mean = np.average(features, axis=0)
    features_var = np.sum((features - features_mean)**2, axis=0)
    features = (features - features_mean) / (features_var) ** 0.5
    plan2 = Plan2(features)
    plan2.fit(learning_rate=args.learning_rate,
              verbose=args.verbose, answer=answer)

    f = open(args.output, 'w')
    ys = plan2.predict()
    for y in ys:
        f.write(str(int(y)) + '\n')
    print(np.sum(ys == answer) / ys.shape[0])

if __name__ == '__main__':
    main()
