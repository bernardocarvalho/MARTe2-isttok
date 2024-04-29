#!/usr/bin/env python3
"""
This script plots the MARTe2 ATCAIop samples stored in MDSplus
All of the plots may be panned/scaled by dragging with 
the left/right mouse buttons. Right click on any plot to show a context menu.
"""
import numpy as np

import pyqtgraph as pg
from pyqtgraph.Qt import QtCore
from MDSplus import Tree
import argparse

app = pg.mkQApp("Plotting MARTe2 AtcaIop Data")
#mw = QtWidgets.QMainWindow()
#mw.resize(800,800)

#MAX_SAMPLES = 50000
ADC_CHANNELS = 14 # channels stored in ISTTOK
DECIM_RATE = 200

parser = argparse.ArgumentParser(description = 'Script to support the QA activities')

    #parser.add_argument('-l','--list', nargs='+', help='<Required> Set flag', required=True)
    #parser.add_argument('-l','--list', nargs='+')
parser.add_argument('-c', '--crange', nargs='+',type=int, help='Channel plots (1 12)',default=[1, 12])
parser.add_argument('-i', '--irange', nargs='+',type=int,default=[1, 12])
#parser.add_argument('pulse','-', nargs='+', help='<Required> Set flag', required=True)
parser.add_argument('-s', '--shot', type=int, help='Mds+ pulse Number ([1, ...])', default=100)
parser.add_argument('-m', '--maxpoints', type=int, help='Max points to plot', default=50000)
parser.add_argument('-e', '--averages', action='store_true', help='Calc averages')
#parser.add_argument('-w', '--drift', action='store_true', help='Calc drifts')
#, default='')
parser.add_argument('-z', '--zero', action='store_true',help='Zero integral Lines') #, default='')

args = parser.parse_args()
mdsPulseNumber = args.shot

mdsTreeName = 'rtappisttok'

try:
    tree = Tree(mdsTreeName, mdsPulseNumber)
except:
    print(f'Failed opening {mdsTreeName} for pulse number {mdsPulseNumber:d}')
    exit()

win = pg.GraphicsLayoutWidget(show=True, title="Basic plotting examples")
win.resize(1000,600)
win.setWindowTitle('pyqtgraph example: Plotting')

# Enable antialiasing for prettier plots
pg.setConfigOptions(antialias=True)

p1 = win.addPlot(title="ATCA-IOP ADC raw decimated")
# add plt.addLegend() BEFORE you create the curves.
#mdsNode = tree.getNode("ATCAIOP1.ADC0RAW")
#dataAdc = mdsNode.getData().data()
#timeData = mdsNode.getDimensionAt(0).data()
p1.addLegend()
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

if(args.averages):
    print(f"EO: {ADC_CHANNELS} ", end='')
    for i in range(ADC_CHANNELS):
        print(f"{meanD[i]:d} ", end='')
    print(" ")
    print(f"WO: {ADC_CHANNELS} ", end='')
    for i in range(ADC_CHANNELS):
        print(f"{driftW[i]:0.3f} ", end='')
    print(" ")
    print(f"Samples {total_samples}, time {total_samples/2e3:.3f} ms")


for i in range(start, stop):
    mdsNode = tree.getNode(f"ATCAIOP1.ADC{i}RAW")
    dataAdc = mdsNode.getData().data()
    timeData = mdsNode.getDimensionAt(0).data()
    y = dataAdc[ :args.maxpoints, 0]
    x = DECIM_RATE * np.arange(len(y)) / 2.0e6
    p1.plot(x,y, pen=pg.mkPen(i, width=2), name=f"Ch {i+1}")
#p1.setLabel('bottom', "Y Axis", units='s')

win.nextRow()
p4 = win.addPlot(title="Channel Integrals")
p4.addLegend()
#for i in range(8,12):
start = args.irange[0] -1; stop = args.irange[1]
#print("WO: ", end='')
for i in range(start, stop):
    #mdsNode = tree.getNode(f"ATCAIOP1.ADC8INT")
    mdsNode = tree.getNode(f"ATCAIOP1.ADC{i}INT")
    try:
        dataAdcInt = mdsNode.getData().data()
        timeData = mdsNode.getDimensionAt(0).data()
        total_samples = DECIM_RATE * len(dataAdcInt[:, 0])
        y = dataAdcInt[ :args.maxpoints, 0]  / 2.0e6 #  LSB * sec
        if(args.zero):
            y = y - dataAdcInt[0, 0] / 2.0e6 #  LSB * sec
            #wo =  (dataAdcInt[-1, 0] - dataAdcInt[0, 0]) /total_samples
            # print(f"{wo:0.4f} ", end='')
        x = DECIM_RATE * np.arange(len(y)) / 2.0e6 # in sec
        p4.plot(x,y, pen=pg.mkPen(i, width=2), name=f"Ch {i+1}")
    except:
        print(f"No data ATCAIOP1.ADC{i:d}INT")

print(" ")

p4.setLabel('bottom', "Time", units='s')

#updatePlot()

if __name__ == '__main__':
    import sys
    if sys.flags.interactive != 1 or not hasattr(QtCore, 'PYQT_VERSION'):
        pg.QtGui.QApplication.exec_()

    #iprint("xwc")
    #pg.exec()
# vim: syntax=python ts=4 sw=4 sts=4 sr et
