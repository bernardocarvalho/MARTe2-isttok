#!/usr/bin/env python3
"""
Extract data from MDSplus and plot
"""
import sys
import numpy as np
import matplotlib.pyplot as plt
from MDSplus import Tree
import argparse


mdsTreeName = 'rtappisttok'

"""
Available Channels

ATCAIOP1.ADC0RAW
ATCAIOP1.ADC1RAW
ATCAIOP1.ADC2RAW
ATCAIOP1.ADC3RAW
ATCAIOP1.ADC4RAW
ATCAIOP1.ADC5RAW
ATCAIOP1.ADC6RAW
#for i in range(args.irange[0], args.irange[1]):
ATCAIOP1.ADC7RAW

ATCAIOP1.ADC0INT
ATCAIOP1.ADC1INT

"""

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description = 'Script to support the QA activities')

    #parser.add_argument('-l','--list', nargs='+', help='<Required> Set flag', required=True)
    #parser.add_argument('-l','--list', nargs='+')
    parser.add_argument('-c', '--crange', nargs='+',type=int, help='Channel plots (1 12)',default=[1, 8])
    parser.add_argument('-i', '--irange', nargs='+',type=int,default=[0, 4])
#parser.add_argument('pulse','-', nargs='+', help='<Required> Set flag', required=True)
    parser.add_argument('-s', '--shot', type=int, help='Mds+ pulse Number ([1, ...])', default=100)

    args = parser.parse_args()
    mdsPulseNumber = args.shot
    try:
        #tree = Tree(mdsTreeName, args.pulse)
        tree = Tree(mdsTreeName, mdsPulseNumber)
    except:
        print(f'Failed opening {mdsTreeName} for pulse number {mdsPulseNumber:d}')
        exit()
    mdsNode = tree.getNode("ATCAIOP1.ADC0RAW")
    mdsNode = tree.getNode("ATCAIOP1.ADC1INT")
    dataAdc = mdsNode.getData().data()
    timeData = mdsNode.getDimensionAt(0).data()



