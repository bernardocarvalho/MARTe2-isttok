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
import argparse
# import matplotlib.pyplot as plt
from MdsImportSdas import StartSdas, LoadSdasData
ISTTOK_RT_PERIOD = 0.0001

def build_import_table_langmuir():
    SDAS_NODEFMT = 'MARTE_NODE_IVO3.DataCollection.Channel_{}'
    LANGMUIR_OFF = 24
    NUM_PROBES = 4
    """
    ADC_electric_top_near: MARTE_NODE_IVO3.DataCollection.Channel_024
    ADC_electric_outer_near: MARTE_NODE_IVO3.DataCollection.Channel_025
    ADC_electric_bottom_near: MARTE_NODE_IVO3.DataCollection.Channel_026
    ADC_electric_inner_near: MARTE_NODE_IVO3.DataCollection.Channel_027
    """
    table = []
    for n in range(NUM_PROBES):
        sdas_str = SDAS_NODEFMT.format(str(LANGMUIR_OFF + n).zfill(3))
        nd = {'sdas': sdas_str, 'name': f'Langmuir Probe no. {n+1}'}
        table.append(nd)
    return table


"""
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
"""


def get_arguments():
    parser = argparse.ArgumentParser(
            description='Import SDAS ISTTOK to csv File ')
    parser.add_argument('-p', '--pulse',
                        help='pulse (shot) number', default='46241', type=int)
    # parser.add_argument('-s', '--shot',
    #                    type=int, help='Mds+ pulse Number ([1, ...])',
    #                  default=100)
    parser.add_argument('-f', '--file', type=str,
                        help='filename device to save', default='LangmuirSdas')
    parser.add_argument('-e', '--exportData',
                        action='store_true', help='Export to MDSPlus')
    parser.add_argument('-m', '--mirnov',
                        action='store_true', help='Import Mirnov')
    parser.add_argument('-l', '--langmuir',
                        action='store_true', help='Import Langmuir')
    parser.add_argument('-t', '--trigger', type=int,
                        help='Trigger sample', default='0')
    parser.add_argument('-n', '--names',
                        action='store_true', help='Print Node Table')
    parser.add_argument('-z', '--zeros', type=int,
                        help='Insert zeros rows', default='0')
    return parser.parse_args()


if (__name__ == "__main__"):
    args = get_arguments()
    pulseNo = args.pulse

    nodeTable = build_import_table_langmuir()
    client = StartSdas()
    langmuirData = []
    for nd in nodeTable:
        print(nd['sdas'])
        data, tzero_us, period = LoadSdasData(client, nd['sdas'], pulseNo)
        langmuirData.append(data)
    time = np.arange(len(data), dtype='uint32') * int(period)
    trigger = np.zeros(len(data), dtype='uint32')
    if args.trigger > 0:
        trigger[args.trigger:] = 1

    langmuirNp = np.array(langmuirData).T
    data2file = np.insert(langmuirNp, 0, trigger,  axis=1)
    data2file = np.insert(data2file, 0, time,  axis=1)
    if args.zeros > 0:
        nCol = data2file.shape[1]
        zerRows = np.zeros([args.zeros, nCol])
        data2file = np.insert(data2file, 0, zerRows, axis=0)
    fname = f"{args.file:s}_{pulseNo}"
    filename = f"{fname}.csv"
#    head = ('#Time (uint32)[1],Langmuir0 (float32)[1],Langmuir1 (float32)[1],'
#            'Langmuir2 (float32)[1],Langmuir3 (float32)[1]')
#    formt = '%d,%.6f,%.6f,%.6f,%.6f'
    formt = '%d,%d,{%.6f,%.6f,%.6f,%.6f}'
    head = '#TimeSdas (uint32)[1],Trigger (uint32)[1],LangmuirSignals (float32)[4]'
    np.savetxt(filename, data2file, fmt=formt,
               header=head, comments='')
    # , delimiter=',')
    np.save(fname, data2file)
#    plot_signals(46241, table)
# formt = ['%d', '%.6f', '%.6f', '%.6f', '%.6f']
