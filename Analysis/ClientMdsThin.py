#!/usr/bin/env python3
"""
This script get data from the MARTe2 ATCAIop stored in MDSplus
"""
import numpy as np
from mdsClient import mdsClient
from mdsthin.exceptions import TreeNNF, TreeFOPENR
import argparse

ADC_CHANNELS = 14  # channels stored in ISTTOK MDSplus trees
ADC_DECIM_RATE = 200  # FPGA decimation

MDSTREENAME = 'isttokmarte'

MDSPLUS_HOST = "192.168.1.173"
# ADC_CHANNELS = 12  # channels stored in ISTTOK MDS
ADC_RAW = '\\TOP.HARDWARE.ATCA_2.IOP_9.CHANNEL_{}.ADC_DECIM'
ADC_RAW_D = '\\TOP.HARDWARE.ATCA_2.IOP_9.CHANNEL_{}.ADC_DECIM_D'

ADC_INTEG = '\\TOP.HARDWARE.ATCA_2.IOP_9.CHANNEL_{}.ADC_INTEG'
SIG_INTEG = "\\TOP.DIAGNOSTICS.MAGNETICS.MIRNOV:PROBE{}.SIG"
ADC_INTEG_D = "\\TOP.HARDWARE.ATCA_2.IOP_9.CHANNEL_{}.ADC_INTEG_D"


class ClientMdsThin():

    """
    Models an MDS Thin Client connection to ISTTOK ATCA2 server
    """

    def __init__(self, shot=52737, num_channels=12):
        """
        Initializes the connection to the High-Precision-AD-HAT
        """
        try:
            self.client = mdsClient(MDSPLUS_HOST, user='oper')
            self.client.openTree(MDSTREENAME, shot)
        except TreeFOPENR:
            print(f"Tree {MDSTREENAME} / Shot {shot} Not found")
            raise ValueError

        self.adcRawData = []
        self.adcIntegData = []

    def getData(self):
        self.adcRawData = []
        self.adcIntegData = []
        for i in range(ADC_CHANNELS):
            data = self.client.getData(ADC_RAW.format(i))
            self.adcRawData.append(data[:, 0])
            data = self.client.getData(ADC_INTEG.format(i))
            self.adcIntegData.append(data[:, 0])

    def calcEoWo(self):
        if not self.adcRawData:
            raise ValueError
        totalSamples = ADC_DECIM_RATE * len(self.adcRawData[0])
        print(f"Samples {totalSamples:d}, time {totalSamples/2e3:.3f} ms")
        Eoffset = np.zeros(ADC_CHANNELS, dtype=int)
        Woffset = np.zeros(ADC_CHANNELS)
        for i in range(ADC_CHANNELS):
            Eoffset[i] = np.mean(self.adcRawData[i]).astype(int)
            intData = self.adcIntegData[i]
            Woffset[i] = (intData[-1] - intData[0]) / totalSamples
        return Eoffset, Woffset



if __name__ == '__main__':
    parser = argparse.ArgumentParser(
            description='Script to get data from remote MDS server')

    # parser.add_argument('-l','--list', nargs='+', help='<Required> Set flag', required=True)
    # parser.add_argument('-l','--list', nargs='+')
    parser.add_argument('-c', '--crange', nargs='+',
                        type=int, help='Channel plots (1 12)', default=[1, 12])
    parser.add_argument('-i', '--irange', nargs='+',
                        type=int, default=[1, 12])
    parser.add_argument('-s', '--shot',
                        type=int, help='Mds+ pulse Number ([1, ...])',
                        default=52737)
    # parser.add_argument('-e', '--averages', action='store_true', help='Calc averages')
# parser.add_argument('-w', '--drift', action='store_true', help='Calc drifts')

    args = parser.parse_args()
    clt = ClientMdsThin(shot=args.shot, num_channels=12)
    #clt.getData()
    # main(args)


# vim: set syntax=python ts=4 sw=4 sts=4 sr et
"""
def main(args):
    mdsPulseNumber = args.shot

    try:
        if (mdsPulseNumber > 0):
            tree = Tree(MDSTREENAME, mdsPulseNumber)
        else:
            tree = Tree(MDSTREENAME)
            mdsPulseNumber = tree.getCurrent()
            tree.close()
            tree = Tree(MDSTREENAME, mdsPulseNumber)
    except Exception:
        print(f'Failed opening {MDSTREENAME} for pulse number ' +
              f'{mdsPulseNumber:d}')
        exit()

    print(f'Opening {MDSTREENAME} for pulse number {mdsPulseNumber:d}')
# add plt.addLegend() BEFORE you create the curves.
# mdsNode = tree.getNode("ATCAIOP1.ADC0RAW")
# dataAdc = mdsNode.getData().data()
# timeData = mdsNode.getDimensionAt(0).data()
    # start = args.crange[0] - 1
    # top = args.crange[1]
# for i in range(args.crange[0], args.crange[1]):
    meanD = np.zeros(ADC_CHANNELS, dtype=int)
    driftW = np.zeros(ADC_CHANNELS)
    total_samples = 0
    for i in range(ADC_CHANNELS):
        mdsNode = tree.getNode(f"ATCAIOP1.ADC{i}RAW")
        dataAdc = mdsNode.getData().data()
        meanD[i] = np.mean(dataAdc[:, 0]).astype(int)
        mdsNode = tree.getNode(f"ATCAIOP1.ADC{i}INT")
        dataAdcInt = mdsNode.getData().data()
        total_samples = DECIM_RATE * len(dataAdcInt[:, 0])
        driftW[i] = (dataAdcInt[-1, 0] - dataAdcInt[0, 0]) / total_samples

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
"""
