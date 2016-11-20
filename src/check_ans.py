import argparse


def main():
    parser = argparse.ArgumentParser(description='calc accuracy')
    parser.add_argument('f1', type=str, help='file 1')
    parser.add_argument('f2', type=str, help='file 2')
    args = parser.parse_args()

    f1 = open(args.f1)
    f2 = open(args.f2)

    ans1 = f1.read().split('\n')
    ans2 = f2.read().split('\n')

    if len(ans1) != len(ans2):
        print('Warining, different len of file:', len(ans1), len(ans2))
        
    correct = 0
    for i in range(len(ans1)):
        if ans1[i] == ans2[i]:
            correct += 1

    print('accuracy:', correct / len(ans1))
    

if __name__ == '__main__':
    main()
