#!/usr/bin/env python3
"""
This script resets the MDSplus pulseNumber of Tree 
"""
from MDSplus import Tree
from MDSplus.mdsExceptions import TreeFOPENR
import argparse


# MDSTREENAME = 'isttokmdsplus'
MDSTREENAME = 'isttokmarte'


def main(args):
    mdsPulseNumber = args.shot

    try:
        if (mdsPulseNumber > 0):
            tree = Tree(MDSTREENAME)
            tree.setCurrent(mdsPulseNumber)
        # else:
            tree.close()

    except TreeFOPENR:
        print(f'Failed opening {MDSTREENAME} for '
              f'pulse number {mdsPulseNumber:d}')
        exit()

    print(f'Last Pulse for {MDSTREENAME} is now {mdsPulseNumber:d}')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
            description='Script to synchronize ISTTOK shot number')

    parser.add_argument('-s', '--shot',
                        type=int, help='Mds+ pulse Number ([1, ...])',
                        default=99)

    args = parser.parse_args()
    main(args)

# vim: set syntax=python ts=4 sw=4 sts=4 sr et
