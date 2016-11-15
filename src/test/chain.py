import numpy as np

def chain(nVars, prior, trans):
    p = np.zeros((nVars, 2))
    p[0] = prior
    for i in range(1, nVars):
        p[i] = prior * np.dot(trans, p[i - 1])
        p[i] /= np.sum(p[i])
    return p
        
def main():
    nVars = 10000
    p = chain(nVars, np.array([1, 3]), np.array([[4, 1], [1, 4]]))
    print(p[-1] / np.sum(p[-1]))

main()
