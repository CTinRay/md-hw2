import argparse

def main():
    parser = argparse.ArgumentParser(description='Generate answer for valid')
    parser.add_argument('response', type=str, help='response.txt')
    parser.add_argument('pred', type=str, help='pred.id')
    parser.add_argument('output', type=str, help='output file')
    args = parser.parse_args()

    truth = set()
    f = open(args.response, 'r')
    for l in f:
        f = int(l.strip().split('\t')[0])
        t = int(l.strip().split('\t')[1])
        truth.add((f, t))

    fpred = open(args.pred, 'r')
    fout = open(args.output, 'w')
    for l in fpred:
        f = int(l.strip().split(' ')[0])
        t = int(l.strip().split(' ')[1])
        if (f, t) in truth:
            fout.write('1\n')
        else:
            fout.write('0\n')
            
            
if __name__ == '__main__':
    main()
