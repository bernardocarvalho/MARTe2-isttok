#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Python3 App to Import data from ISTTOK SDAS to MDSPlus
author:  B. Carvalho / IPFN-IST
email: bernardo.carvalho@tecnico.ulisboa.pt

Install sdas:
http://metis.ipfn.tecnico.ulisboa.pt/CODAC/IPFN_Software/SDAS/Access/Python
"""

import numpy as np
import matplotlib.pyplot as plt
from sdas.core.client.SDASClient import SDASClient
from sdas.core.SDAStime import Date, Time, TimeStamp
from MDSplus import (Tree,
                     Signal,
                     makeArray,
                     Float64,
                     Float32Array,
                     Range)
from MDSplus import mdsExceptions

MDSTREENAME = 'isttoksdas'

SEGMENT_SIZE = 100
ISTTOK_RT_PERIOD = 0.0001


def StartSdas():
    host = 'baco.ipfn.ist.utl.pt'
    port = 8888
    client = SDASClient(host, port)
    return client


def LoadSdasData(client, channelID, shotnr):
    dataStruct = client.getData(channelID, '0x0000',
                                shotnr)
    dataArray = dataStruct[0].getData()
    len_d = len(dataArray)
    tstart = dataStruct[0].getTStart()
    tend = dataStruct[0].getTEnd()
    period = (tend.getTimeInMicros() - tstart.getTimeInMicros())*1.0/len_d
    # print(f'Period: {period}')
    events = dataStruct[0].get('events')[0]
    tevent = TimeStamp(tstamp=events.get('tstamp'))
    # tstart_us = tstart.getTimeInMicros()
    tzero_us = tstart.getTimeInMicros() - tevent.getTimeInMicros()
    return dataArray, tzero_us, period
    # timeVector = np.linspace(delay, delay+period*(len_d-1), len_d)
    # return [dataArray, timeVector]


def ForceSegment(nd, start, period, data):
    end = start + (SEGMENT_SIZE - 1) * period
    dimension = Range(Float64(start), Float64(end),
                      Float64(period))
    nd.beginSegment(Float64(start), Float64(end),
                    dimension, Float32Array(data))


def plot_signals(pulse, nodeTable):
    try:
        tree = Tree(MDSTREENAME, pulse)
    except mdsExceptions.TreeNOPATH:
        print(f'Failed opening {MDSTREENAME}')
    except mdsExceptions.TreeFOPENR:
        print(f'Failed opening {MDSTREENAME} for pulse number {pulse:d}')
        exit()
    NoOfColors = len(nodeTable)
    cm = plt.get_cmap('jet')
    fig = plt.figure(figsize=(5, 7), tight_layout=True)
    ax = fig.add_subplot(111)
    ax.set_prop_cycle(color=[cm(1.*i/NoOfColors) for i in range(NoOfColors)])
    plt.title(f'#{pulse}: Mirnov coil signals')

    for nd in nodeTable:
        nd_mds = tree.getNode(nd['mds'])
        mdsData = nd_mds.getData()
        signal = mdsData.data()
        if len(signal) > 0:
            times = mdsData.dims[0]
            plt.plot(times, signal, label=nd['name'])

    plt.xlabel('Time / s ')
    plt.legend()
    plt.grid(True)
    plt.show()


def mds_writer(pulse, import_table):
    try:
        tree = Tree(MDSTREENAME, pulse)
    except mdsExceptions.TreeNOPATH:
        print(f'Failed opening {MDSTREENAME}')
    except mdsExceptions.TreeFOPENR:
        print(f'Failed opening {MDSTREENAME} for pulse number {pulse:d}.'
              ' Trying to create')
        # print(f'Failed opening {MDSTREENAME}')
        # exit()
        try:
            # tree.open(shot=pulse)
            tree = Tree(MDSTREENAME, -1)
            # Tree.setCurrent(MDSTREENAME, pulse)
            tree.createPulse(pulse)
            tree.close()
            tree = Tree(MDSTREENAME, pulse)
        except mdsExceptions.TreeFOPENR:
            print(f'Failed create pulse {MDSTREENAME}')
            exit()

    client = StartSdas()
    for nd in import_table:
        print(nd['sdas'])
        print(nd['mds'])
        nd_mds = tree.getNode(nd['mds'])
        data, tzero_us, period = LoadSdasData(client,
                                              nd['sdas'], pulse)
        # print(tzero_us)
        # Period = period * 1e-6
        Period = ISTTOK_RT_PERIOD
        print(f"period: {Period}")
        Start = tzero_us * 1e-6  # in us
        count = 0
        segData = np.zeros(SEGMENT_SIZE, dtype=np.float32)
        for dt in data:
            segData[count] = dt
            count += 1
            if (count >= SEGMENT_SIZE):
                ForceSegment(nd_mds, Start, Period, segData)
                Start += SEGMENT_SIZE * Period
                count = 0


if (__name__ == "__main__"):
    table = [{'sdas': 'MARTE_NODE_IVO3.DataCollection.Channel_166', 'mds': '\\TOP.HARDWARE.ATCA1.IOC1.CHANNELS.INPUT_00.RAW', 'name': 'Mirnov coil no. 1'},
             {'sdas': 'MARTE_NODE_IVO3.DataCollection.Channel_167', 'mds': '\\TOP.HARDWARE.ATCA1.IOC1.CHANNELS.INPUT_01.RAW', 'name': 'Mirnov coil no. 2'},
             {'sdas': 'MARTE_NODE_IVO3.DataCollection.Channel_168', 'mds': '\\TOP.HARDWARE.ATCA1.IOC1.CHANNELS.INPUT_02.RAW', 'name': 'Mirnov coil no. 3'},
             {'sdas': 'MARTE_NODE_IVO3.DataCollection.Channel_169', 'mds': '\\TOP.HARDWARE.ATCA1.IOC1.CHANNELS.INPUT_03.RAW', 'name': 'Mirnov coil no. 4'},
             {'sdas': 'MARTE_NODE_IVO3.DataCollection.Channel_170', 'mds': '\\TOP.HARDWARE.ATCA1.IOC1.CHANNELS.INPUT_04.RAW', 'name': 'Mirnov coil no. 5'},
             {'sdas': 'MARTE_NODE_IVO3.DataCollection.Channel_171', 'mds': '\\TOP.HARDWARE.ATCA1.IOC1.CHANNELS.INPUT_05.RAW', 'name': 'Mirnov coil no. 6'}]
    # mds_writer(100, table)
    plot_signals(46241, table)
