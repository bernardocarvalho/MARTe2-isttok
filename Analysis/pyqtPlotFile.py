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
# from pyqtgraph.Qt import QtCore
import argparse

app = pg.mkQApp("Plotting MARTe2 AtcaIop Data")
# mw = QtWidgets.QMainWindow()
# mw.resize(800,800)

# MAX_SAMPLES = 50000
ADC_CHANNELS = 14  # channels stored in ISTTOK
ADC_DECIM_RATE = 200

parser = argparse.ArgumentParser(description=
        """Script to plot AtcaIop MDSplus data and calc drifts
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
                    help='Max points to plot', default=1000)
parser.add_argument('-e', '--averages', action='store_true',
                    help='Calc averages')
parser.add_argument('-d', '--decimated', action='store_true',
                    help='Use decimated data')
# parser.add_argument('-w', '--drift', action='store_true', help='Calc drifts')
parser.add_argument('-z', '--zero', action='store_true',
                    help='Zero integral Lines')
parser.add_argument('-f', '--file', type=str,
                        help='filename device to open', default='LangmuirSdas')

args = parser.parse_args()


win = pg.GraphicsLayoutWidget(show=True, title="Basic plotting examples")
win.resize(1000, 600)
win.setWindowTitle('ISTTOK Plotting')

# Enable antialiasing for prettier plots
pg.setConfigOptions(antialias=True)

p1 = win.addPlot(title="SDAS Languir Probe data")
# add plt.addLegend() BEFORE you create the curves.
# mdsNode = tree.getNode("ATCAIOP1.ADC0RAW")
# dataAdc = mdsNode.getData().data()
# timeData = mdsNode.getDimensionAt(0).data()
p1.addLegend()
start = args.crange[0] - 1
stop = args.crange[1]

dataSdas = np.load(args.file)
x = dataSdas[:args.maxpoints, 0] / 1e3  # us -> ms

# for i in range(1, 2):

lineName = ['Time', 'Trigger',
            'ElectricTop', 'ElectricInner', 'ElectricOuter', 'ElectricBottom']
for i in [2, 3, 4, 5]:
    y = dataSdas[:args.maxpoints, i]
    # p1.plot(y, pen=pg.mkPen(i, width=2), name=f"Ch {i}")
    p1.plot(y, pen=pg.mkPen(i, width=2), name=lineName[i])

p1.showGrid(x=True, y=True, alpha=0.5)
# p1.setLabel('bottom', "Time", units='ms')
# p1.setLabel('bottom', "Samp", units='')

win.nextRow()

p2 = win.addPlot(title="MARTE2 output data")
dataOut = np.genfromtxt('../Startup/IsttokOutput.csv', delimiter=',')
p2.addLegend()
x = dataOut[:args.maxpoints, 0] / 1e3  # us -> ms
y = dataOut[:args.maxpoints, 1]
# for i in range(1, 5):
lineName = ['Time', 'OutMdsW0', 'OutMdsW1', 'OutMdsW2', 'OutMdsW3']
for i in [1, 2, 3, 4,]:
    y = dataOut[:args.maxpoints, i]
    p2.plot(y, pen=pg.mkPen(i, width=2), name=lineName[i])

p2.showGrid(x=True, y=True, alpha=0.5)
# p2.plot(x, y)  # , pen=pg.mkPen(i, width=2))  # , name=f"Ch {i+1}")

p2.setLabel('bottom', "Time", units='ms')
# print("WO: ", end='')
# updatePlot()

if __name__ == '__main__':
    # import sys
    # if sys.flags.interactive != 1 or not hasattr(QtCore, 'PYQT_VERSION'):
    #    pg.QtGui.QApplication.exec_()

    # iprint("xwc")
    pg.exec()
# vim: syntax=python ts=4 sw=4 sts=4 sr et
