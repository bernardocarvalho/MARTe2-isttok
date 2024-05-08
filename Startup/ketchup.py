
#!/usr/bin/env python3
"""
This script 
"""

from MDSplus import Tree
import argparse


MDSTREENAME = 'rtappisttok'

def main(args):
    mdsPulseNumber = args.shot

    try:
        if(mdsPulseNumber > 0):
            tree = Tree(MDSTREENAME)
            tree.setCurrent(mdsPulseNumber)
        #else:
            tree.close()

    except:
        print(f'Failed opening {MDSTREENAME} for pulse number {mdsPulseNumber:d}')
        exit()

    print(f'Last Pulse for {MDSTREENAME}  is {mdsPulseNumber:d}')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description = 'Script to synchronize ISTTOK shot number')

    parser.add_argument('-s', '--shot', type=int, help='Mds+ pulse Number ([1, ...])', default=100)

    args = parser.parse_args()
    main(args)
# vim: set syntax=python ts=4 sw=4 sts=4 sr et
