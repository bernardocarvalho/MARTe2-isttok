#!/usr/bin/env python3
"""
This script plots the MARTe2 ATCAIop samples stored in MDSplus
"""
# import numpy as np

# from pyqtgraph.Qt import QtWidgets, mkQApp
import argparse
from ClientMdsThin import ClientMdsThin as cltMds

ADC_CHANNELS = 16  # channels stored in ISTTOK MDS
# ADC_BIT_LSHIFT = 2**14
MDS_URL = "oper@atca-marte2"

ADC_CHANNELS = 14  # channels stored in ISTTOK
# DECIM_RATE = 200

MDSTREENAME = 'rtappisttok'


def main(args):
    mdsPulseNumber = args.shot

    try:
        mclient = cltMds(url=args.url, num_channels=ADC_CHANNELS)
        mclient.getTreeData(shot=args.shot)
#        if (mdsPulseNumber > 0):
#            tree = Tree(MDSTREENAME, mdsPulseNumber)
#        else:
#            tree = Tree(MDSTREENAME)
#            mdsPulseNumber = tree.getCurrent()
#            tree.close()
#            tree = Tree(MDSTREENAME, mdsPulseNumber)

    except Exception:
        print(f'Failed opening {MDSTREENAME} for pulse number ' +
              f'{mdsPulseNumber:d}')
        exit()

    print(f'Opening {MDSTREENAME} for pulse number {mdsPulseNumber:d}')
    Eoffset, Woffset = mclient.calcEoWo()
    #meanD = np.zeros(ADC_CHANNELS, dtype=int)
    #driftW = np.zeros(ADC_CHANNELS)
    # total_samples = 0
    print(f"caput -a ISTTOK:central:ATCAIOP1-EO {ADC_CHANNELS} ", end='')
    for i in range(ADC_CHANNELS):
        print(f"{Eoffset[i]:d} ", end='')
    print(" ")
    #print(f"WO: {ADC_CHANNELS} ", end='')
    print(f"caput -a ISTTOK:central:ATCAIOP1-WO {ADC_CHANNELS} ", end='')
    for i in range(ADC_CHANNELS):
        print(f"{Woffset[i]:0.3f} ", end='')
    print(" ")
    # print(f"Samples {total_samples}, time {total_samples/2e3:.2f} ms")


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
            description='Script to support the QA activities')

    # parser.add_argument('-l','--list', nargs='+', help='<Required> Set flag', required=True)
    # parser.add_argument('-l','--list', nargs='+')
    parser.add_argument('-c', '--crange', nargs='+',
                        type=int, help='Channel plots (1 12)', default=[1, 12])
    parser.add_argument('-i', '--irange', nargs='+',
                        type=int, default=[1, 12])
    parser.add_argument('-s', '--shot',
                        type=int, help='Mds+ pulse Number ([1, ...])',
                        default=0)
    parser.add_argument('-u', '--url',  default=MDS_URL,
                        help='MDSplus host url')
    # parser.add_argument('-e', '--averages', action='store_true', help='Calc averages')
# parser.add_argument('-w', '--drift', action='store_true', help='Calc drifts')

    args = parser.parse_args()
    main(args)
# vim: set syntax=python ts=4 sw=4 sts=4 sr et
