import numpy as np
import argparse
from sklearn.covariance import EllipticEnvelope 
from sklearn.ensemble import IsolationForest

def load_feature(filename):
    f = open(filename)
    features = []
    for l in f:
        features += [list(map(float, l.strip().split(' ')))]
    return np.array(features)
    

def main():
    parser = argparse.ArgumentParser(description='===== PLAN III =====')
    parser.add_argument('features', type=str, help='features')
    parser.add_argument('output', type=str, help='output')
    parser.add_argument('--converge', type=float, help='converge, default = 0.001', default=0.001)
    parser.add_argument('--verbose', type=bool, help='verbose, default = False', default=False)    
    parser.add_argument('--learning_rate', type=float, help='learning rate, default = 0.01', default=0.001)    
    parser.add_argument('--answer', type=str, help='answer')    
    
    args = parser.parse_args()

    answer = None
    if args.answer is not None:
        f = open(args.answer)
        answer = np.array(list(map(int, f.read().strip().split('\n'))), dtype=int)
        
    features = load_feature(args.features) #[:,[0, 1, 3, 4, 6]]
    # do normalization
    features_mean = np.average(features, axis=0)
    features_var = np.sum((features - features_mean)**2, axis=0)
    features = (features - features_mean) / (features_var) ** 0.5

    # todo
    ee = IsolationForest()
    ee.fit(features)
    ys = ee.predict()

    f = open(args.output, 'w')
    for y in ys:
        f.write(str(int(y)) + '\n')
    print(np.sum(ys == answer) / ys.shape[0])

if __name__ == '__main__':
    main()
