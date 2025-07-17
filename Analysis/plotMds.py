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
from pyqtgraph.Qt import QtWidgets, mkQApp
# from mdsClient import mdsClient
# from mdsthin.exceptions import TreeNNF
import argparse

from ClientMdsThin import ClientMdsThin as cltMds

ADC_CHANNELS = 16  # channels stored in ISTTOK MDS
ADC_BIT_LSHIFT = 2**14
MDS_URL = "oper@atca-marte2"

class MainWindow(QtWidgets.QMainWindow):
    """ main window """
    def __init__(self, *args, **kwargs):
        super(MainWindow, self).__init__(*args, **kwargs)
        parser = argparse.ArgumentParser(
            description="""Script to plot AtcaIop MDSplus data and calc drifts
            """)

        parser.add_argument('-s', '--shot', type=int,
                            help='Mds+ pulse Number ([1, ...])', default=100)
        parser.add_argument('-c', '--crange', nargs='+', type=int,
                    help='Channel plots (1 16)', default=[1, 12])
        #parser.add_argument('-n', '--nchannels', type=int,
        #                    help='Number of channels)', default=ADC_CHANNELS)
        parser.add_argument('-m', '--maxpoints', type=int,
                            help='Max points to plot', default=50000)
        parser.add_argument('-y', '--yrange', type=float,
                            help='Integ Y Range', default=0.0)
        parser.add_argument('-a', '--averages', action='store_true',
                            help='calc Averages')
        # parser.add_argument('-d', '--decimated', action='store_true',
        #                     help='Use decimated data')
        # parser.add_argument('-w', '--drift', action='store_true', help='Calc drifts')
        parser.add_argument('-z', '--zero', action='store_true',
                            help='Zero integral Lines')

        parser.add_argument('-u', '--url',  default=MDS_URL,
                                help='MDSplus host url')

        args = parser.parse_args()
        print(f"args.shot {args.shot}")
        self.mclient = cltMds(url=args.url, num_channels=ADC_CHANNELS)
        self.mclient.getTreeData(shot=args.shot)
        # gr_wid = pg.GraphicsLayoutWidget(show=True)
        cw = QtWidgets.QWidget()
        glay = QtWidgets.QGridLayout()
        cw.setLayout(glay)
        # layout.setSpacing(0)
        # layout.setColumnStretch(layout.columnCount(), 1)
        pw1 = pg.PlotWidget(name='Plot1')  ## giving the plots names allows us to link their axes
        pw1.addLegend()
        glay.addWidget(pw1, 0, 0)
        # self.setCentralWidget(gr_wid)
        self.setCentralWidget(cw)
        self.setWindowTitle(f"pyqtgraph ISTTOK shot: {args.shot}")

        # p1 = gr_wid.addPlot(0,0, 1,1, title="raw data")
        start_ch = args.crange[0] - 1
        end_ch = args.crange[1]
        if (args.averages):
            Eoffset, Woffset = self.mclient.calcEoWo()
            # nChannels = len(mclient.adcRawData)
            print(f"EO: {ADC_CHANNELS} ", end='')
            for i in range(end_ch):
                print(f"{Eoffset[i]:d} ", end='')
            print(" ")
            print(f"WO: {ADC_CHANNELS} ", end='')
            for i in range(end_ch):
                print(f"{Woffset[i]:0.3f} ", end='')
            print(" ")
        time = self.mclient.timeR
        for i in range(start_ch, end_ch):
            dataAdc = self.mclient.adcRawData[i]
            pw1.plot(dataAdc[:args.maxpoints], pen=pg.mkPen(i, width=1),
                     name="ch {}".format(i + 1))

        pw2 = pg.PlotWidget(name='Integ data')
        pw2.addLegend()
        pw2.setXLink('Plot1')
        glay.addWidget(pw2, 1, 0)
        # piw2 = gr_wid.addPlot(1,0, 1,1, title="integ data")

        time = self.mclient.timeI
        for i in range(start_ch, end_ch):
            data = self.mclient.adcIntegData[i]
            if (args.zero):
                data -= data[0]  # / 2.0e6  # LSB * sec
            pw2.plot(data[:args.maxpoints], pen=pg.mkPen(i, width=1),
                     name="ch {}".format(i + 1))

        ylim = args.yrange
        if (ylim > 0.0):
            print(f"Yrange: {ylim}")
            pw2.setYRange(-ylim, ylim, padding=0)

        # sub1 = gr_wid.addLayout(0,2, 1,2)
        # piw2 = gr_wid.addPlot(1,0, 1,1, title="integ data")

        # time = self.mclient.timeI
        # for i in range(6):
        #    data = self.mclient.adcIntegData[i]
        #    pw2.plot(data, pen=pg.mkPen(i, width=1))
            #, pen=(255,0,0), name="Red curve")

        # sub1 = gr_wid.addLayout(0,2, 1,2)
        pw3 = pg.PlotWidget(name='Chop Trigger')
        pw3.setXLink('Plot1')
        data = self.mclient.choppTrigg
        pw3.plot(data[:args.maxpoints], pen=pg.mkPen(i, width=1))
        pw3.setYRange(0, 4, padding=0)
        glay.addWidget(pw3, 2, 0)
        glay.setRowStretch(0, 1)
        glay.setRowStretch(1, 2)
        glay.setRowStretch(2, 1)

        self.resize(900, 800)
        self.show()

mkQApp("ColorBarItem Example")
main_window = MainWindow()

## Start Qt event loop
if __name__ == '__main__':
    pg.exec()
      ## Create image items

"""
        l = QtWidgets.QVBoxLayout()
        # sub1.addLabel("<b>Standard mouse interaction:</b><br>left-drag to pan, right-drag to zoom.")
        l.addWidget(QtWidgets.QLabel("<b>Standard mouse interaction:</b><br>left-drag to pan, right-drag to zoom."))
        saveBtn = QtWidgets.QPushButton('Channel')
        saveBtn.setCheckable(True)
        saveBtn.clicked.connect(lambda x: self.my_custom_fn(x, 25))

        l.addWidget(saveBtn)
        # sub1.addItem(saveBtn)
        glay.addLayout(l, 0, 2)
    # SLOT: This has default parameters and can be called without a value
    def my_custom_fn(self, a="HELLLO!", b=5):
        print(a, b)


"""
