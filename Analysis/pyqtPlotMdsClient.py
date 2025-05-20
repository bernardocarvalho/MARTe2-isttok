#!/usr/bin/env python3
"""
This script plots the MARTe2 ATCAIop samples stored in MDSplus
All of the plots may be panned/scaled by dragging with
the left/right mouse buttons. Right click on any plot to show a context menu.

To change EPICS EO, WO use. e.g.
caput -a ISTTOK:central:ATCAIOP1-WO 14 0.191 0.174 -0.036 -0.044 0.183 0.126 0.020 0.140 -0.461 -0.572 0.022 -0.262 0.475 0.353

"""
import numpy as np

import pyqtgraph as pg
from mdsClient import mdsClient
from mdsthin.exceptions import TreeNNF
import argparse

from ClientMdsThin import ClientMdsThin as Mclient

app = pg.mkQApp("Plotting MARTe2 AtcaIop Data")
# mw = QtWidgets.QMainWindow()
# mw.resize(800,800)

# MAX_SAMPLES = 50000
MDSPLUS_HOST = "192.168.1.173"
ADC_CHANNELS = 14  # channels stored in ISTTOK MDS
ADC_DECIM_RATE = 200  # FPGA decimation
ADC_RAW = '\\TOP.HARDWARE.ATCA_2.IOP_9.CHANNEL_{}.ADC_DECIM'
ADC_RAW_D = '\\TOP.HARDWARE.ATCA_2.IOP_9.CHANNEL_{}.ADC_DECIM_D'

ADC_INTEG = '\\TOP.HARDWARE.ATCA_2.IOP_9.CHANNEL_{}.ADC_INTEG'
SIG_INTEG = "\\TOP.DIAGNOSTICS.MAGNETICS.MIRNOV:PROBE{}.SIG"
ADC_INTEG_D = '\\TOP.HARDWARE.ATCA_2.IOP_9.CHANNEL_{}.ADC_INTEG_D'

parser = argparse.ArgumentParser(
        description="""Script to plot AtcaIop MDSplus data and calc drifts
        To change EPICS EO, WO use. e.g.
        caput -a ISTTOK:central:ATCAIOP1-WO 14 0.191 0.174 -0.036 -0.044 0.183 0.126 0.020 0.140 -0.461 -0.572 0.022 -0.262 0.475 0.353""")

# parser.add_argument('-l','--list', nargs='+',
# help='<Required> Set flag', required=True)
# parser.add_argument('-l','--list', nargs='+')
parser.add_argument('-c', '--crange', nargs='+', type=int,
                    help='Channel plots (1 12)', default=[1, 12])
parser.add_argument('-i', '--irange', nargs='+', type=int, default=[1, 12])
# arser.add_argument('pulse','-', nargs='+', help='<Required> Set flag', required=True)
parser.add_argument('-s', '--shot', type=int,
                    help='Mds+ pulse Number ([1, ...])', default=100)
parser.add_argument('-m', '--maxpoints', type=int,
                    help='Max points to plot', default=50000)
parser.add_argument('-a', '--averages', action='store_true',
                    help='calc Averages')
parser.add_argument('-d', '--decimated', action='store_true',
                    help='Use decimated data')
# parser.add_argument('-w', '--drift', action='store_true', help='Calc drifts')
parser.add_argument('-z', '--zero', action='store_true',
                    help='Zero integral Lines')

args = parser.parse_args()
mdsPulseNumber = args.shot

mclient = Mclient(shot=args.shot)
mclient.getData()

# mdsTreeName = 'rtappisttok'
mdsTreeName = 'isttokmarte'

client = mdsClient(MDSPLUS_HOST, user='oper')
client.openTree(mdsTreeName, args.shot)

win = pg.GraphicsLayoutWidget(show=True, title="Basic plotting examples")
win.resize(1000, 600)
win.setWindowTitle('pyqtgraph example: Plotting')

# Enable antialiasing for prettier plots
pg.setConfigOptions(antialias=True)

p1 = win.addPlot(title="ATCA-IOP ADC raw data")
p1.setDownsampling(ds=200, auto=False)
p1.showGrid(x=True, y=True)  # , alpha=0.5)
# add plt.addLegend() BEFORE you create the curves.
# mdsNode = tree.getNode("ATCAIOP1.ADC0RAW")
# dataAdc = mdsNode.getData().data()
# timeData = mdsNode.getDimensionAt(0).data()
p1.addLegend()
start = args.crange[0] - 1
stop = args.crange[1]
# for i in range(args.crange[0], args.crange[1]):
meanD = np.zeros(ADC_CHANNELS, dtype=int)
# driftW = np.zeros(ADC_CHANNELS)

if (args.averages):
    Eoffset, Woffset = mclient.calcEoWo()
    nChannels = len(mclient.adcRawData)
    print(f"EO: {ADC_CHANNELS} ", end='')
    for i in range(nChannels):
        print(f"{Eoffset[i]:d} ", end='')
    print(" ")
    print(f"WO: {ADC_CHANNELS} ", end='')
    for i in range(ADC_CHANNELS):
        print(f"{Woffset[i]:0.3f} ", end='')
    print(" ")
    # print(f"Samples {total_samples}, time {total_samples/2e3:.3f} ms")


for i in range(start, stop):
    # mdsNode = tree.getNode(f"ATCAIOP1.ADC{i}RAW")
    dataAdc = client.getData(ADC_RAW.format(i))
    # dataAdc = mdsNode.getData().data()
    # timeData = mdsNode.getDimensionAt(0).data()
    timeData = client.getTime(ADC_RAW.format(i))
    y = dataAdc[:args.maxpoints, 0]
    x = ADC_DECIM_RATE * np.arange(len(y)) / 2.0e6
    p1.plot(x, y, pen=pg.mkPen(i, width=2), name=f"Ch {i+1}")
# p1.setLabel('bottom', "Y Axis", units='s')

win.nextRow()
p2 = win.addPlot(title="Channel Integrals")
# p2.setDownsampling(ds=200, auto=False)
p2.showGrid(x=True, y=True)  # , alpha=0.5)
p2.addLegend()
start = args.irange[0] - 1
stop = args.irange[1]
# print("WO: ", end='')
# dataIntAll = []
for i in range(start, stop):
    # mdsNode = tree.getNode(f"ATCAIOP1.ADC8INT")
    if args.decimated:
        node = ADC_INTEG_D.format(i)
    else:
        # node = f"ATCAIOP1.ADC{i}INT"
        node = SIG_INTEG.format(i + 1)
    # mdsNode = tree.getNode(node)
    try:
        dataAdcInt = client.getData(node)
        # dataIntAll.append(dataAdcInt[:, 0])
        timeData = client.getTime(node)
        total_samples = len(dataAdcInt[:, 0])
        y = dataAdcInt[:args.maxpoints, 0]  #  / 2.0e6  # LSB * sec
        x = np.arange(len(y)) / 2.0e6  # in sec
        # if args.decimated:
        total_samples *= ADC_DECIM_RATE
        x *= ADC_DECIM_RATE
        if (args.zero):
            y = y - dataAdcInt[0, 0] # / 2.0e6  # LSB * sec
            # print(f"{wo:0.4f} ", end='')
        p2.plot(x, y, pen=pg.mkPen(i, width=2), name=f"Ch {i+1}")
    except Exception:
        print(f"No data for {node}")

print(" ")

p2.setLabel('bottom', "Time", units='s')
p2.setLabel('left', "Integ", units='lsb.s')
p2.setDownsampling(ds=200, auto=False)
# p2.setXRange(0, 2, padding=0)
# p2.setYRange(-4, 4)

# updatePlot()

if __name__ == '__main__':
    # import sys
    # if sys.flags.interactive != 1 or not hasattr(QtCore, 'PYQT_VERSION'):
    #    pg.QtGui.QApplication.exec_()

    # iprint("xwc")
    pg.exec()
# vim: syntax=python ts=4 sw=4 sts=4 sr et

"""
total_samples = 0
for i in range(ADC_CHANNELS):
    dataAdc = client.getData(ADC_RAW.format(i))
    meanD[i] = np.mean(dataAdc[:, 0]).astype(int)
    dataAdcInt = client.getData(ADC_INTEG.format(i))
    total_samples = ADC_DECIM_RATE * len(dataAdcInt[:, 0])
    # driftW[i] = (dataAdcInt[-1, 0] - dataAdcInt[0, 0]) / total_samples
def getMdsData(tree, node):
    try:
        mdsNode = tree.getNode(node)
        data = mdsNode.getData().data()
    # except Exception:
    except mds.mdsExceptions.TreeNODATA:
        print(f'Failed getMdsData for note {node:s}')
        exit()

    return data


try:
    tree = mds.Tree(mdsTreeName, mdsPulseNumber)
# except Exception:
except mds.mdsExceptions.TreeFOPENR:
    print(f'Failed opening {mdsTreeName} for pulse number {mdsPulseNumber:d}')
    exit()
"""

