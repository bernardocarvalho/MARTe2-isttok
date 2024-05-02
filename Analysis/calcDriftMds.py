#!/usr/bin/env python3
"""
This script plots the MARTe2 ATCAIop samples stored in MDSplus
All of the plots may be panned/scaled by dragging with 
the left/right mouse buttons. Right click on any plot to show a context menu.
"""
import numpy as np

from MDSplus import Tree
import argparse

ADC_CHANNELS = 14 # channels stored in ISTTOK
DECIM_RATE = 200

MDSTREENAME = 'rtappisttok'

def main(args):
    mdsPulseNumber = args.shot

    try:
        if(mdsPulseNumber > 0):
            tree = Tree(MDSTREENAME, mdsPulseNumber)
        else:
            tree = Tree(MDSTREENAME)
            mdsPulseNumber = tree.getCurrent()
            tree.close()
            tree = Tree(MDSTREENAME, mdsPulseNumber)

    except:
        print(f'Failed opening {MDSTREENAME} for pulse number {mdsPulseNumber:d}')
        exit()

    print(f'Openpening {MDSTREENAME} for pulse number {mdsPulseNumber:d}')
# add plt.addLegend() BEFORE you create the curves.
#mdsNode = tree.getNode("ATCAIOP1.ADC0RAW")
#dataAdc = mdsNode.getData().data()
#timeData = mdsNode.getDimensionAt(0).data()
    start = args.crange[0] -1; stop = args.crange[1]
#for i in range(args.crange[0], args.crange[1]):
    meanD = np.zeros(ADC_CHANNELS, dtype=int)
    driftW = np.zeros(ADC_CHANNELS)
    total_samples = 0
    for i in range(ADC_CHANNELS):
        mdsNode = tree.getNode(f"ATCAIOP1.ADC{i}RAW")
        dataAdc = mdsNode.getData().data()
        meanD[i] = np.mean(dataAdc[:,0]).astype(int)
        mdsNode = tree.getNode(f"ATCAIOP1.ADC{i}INT")
        dataAdcInt = mdsNode.getData().data()
        total_samples = DECIM_RATE * len(dataAdcInt[:, 0])
        driftW[i] =  (dataAdcInt[-1, 0] - dataAdcInt[0, 0]) /total_samples

   # if(args.averages):
    print(f"EO: {ADC_CHANNELS} ", end='')
    for i in range(ADC_CHANNELS):
        print(f"{meanD[i]:d} ", end='')
    print(" ")
    print(f"WO: {ADC_CHANNELS} ", end='')
    for i in range(ADC_CHANNELS):
       print(f"{driftW[i]:0.3f} ", end='')
    print(" ")
    print(f"Samples {total_samples}, time {total_samples/2e3:.2f} ms")


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description = 'Script to support the QA activities')

    #parser.add_argument('-l','--list', nargs='+', help='<Required> Set flag', required=True)
    #parser.add_argument('-l','--list', nargs='+')
    parser.add_argument('-c', '--crange', nargs='+',type=int, help='Channel plots (1 12)',default=[1, 12])
    parser.add_argument('-i', '--irange', nargs='+',type=int,default=[1, 12])
    parser.add_argument('-s', '--shot', type=int, help='Mds+ pulse Number ([1, ...])', default=0)
    #parser.add_argument('-e', '--averages', action='store_true', help='Calc averages')
#parser.add_argument('-w', '--drift', action='store_true', help='Calc drifts')
#, default='')

    args = parser.parse_args()
    main(args)
# vim: set syntax=python ts=4 sw=4 sts=4 sr et
