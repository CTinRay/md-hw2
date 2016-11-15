import numpy as np
import argparse

class Plan2:    

    def __init__(self, features):
        self.features = features
        self.weights = np.empty(features.shape[1])
        self.weights.fill(1)

        
    def iteration(self, learning_rate=0.01, verbose=False, answer=None):
        scores = np.sum(self.weights * self.features, axis=1)
        median = np.median(scores)
        upper = np.where(scores > median)[0]
        whole = np.arange(self.features.shape[0])

        upper = np.random.choice(upper, 1000)
        whole = np.random.choice(whole, 2000)

        feature_upper = np.sum(self.features[upper], axis=0)
        feature_whole = np.sum(self.features[whole], axis=0)        
        score_upper = np.sum(scores[upper])
        score_whole = np.sum(scores[whole])
        gradient = (feature_upper * score_whole - feature_whole * score_upper) / score_whole ** 2

        if verbose is True:
            print('|gradient| =', np.linalg.norm(gradient))
            print('score_upper / scopre_whole =', score_upper / score_whole)
            # print('weights:', self.weights)
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
        
    features = load_feature(args.features)[:,[0, 1, 2, 3, 4, 5, 6]]
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
