#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Python3 App to Import Magnetic data from ISTTOK SDAS to MDSPlus
author:  B. Carvalho / IPFN-IST
email: bernardo.carvalho@tecnico.ulisboa.pt

Install sdas:
http://metis.ipfn.tecnico.ulisboa.pt/CODAC/IPFN_Software/SDAS/Access/Python
"""

import argparse
# import numpy as np

from MdsImportSdas import mds_writer, plot_signals

mdsTreeName = 'isttoksdas'


MDS_NODEFMT = "\\TOP.HARDWARE.ATCA1.IOC_0.CHANNEL_{}.RAW"
# MDS_NODEFMT = "\\TOP.HARDWARE.ATCA1.IOC1.CHANNELS.INPUT_{}.RAW"
SDAS_NODEFMT = 'MARTE_NODE_IVO3.DataCollection.Channel_{}'

NUM_PROBES = 12


def build_import_table():
    table = []
    for n in range(NUM_PROBES):
        sdas_str = SDAS_NODEFMT.format(str(166 + n).zfill(3))
        # mds_str = MDS_NODEFMT.format(str(n).zfill(2))
        mds_str = MDS_NODEFMT.format(str(n))
        # print(sdas_str)
        nd = {'sdas': sdas_str, 'mds': mds_str,
              'name': f'Mirnov coil no. {n+1}'}
        table.append(nd)
    return table


def get_arguments():
    parser = argparse.ArgumentParser(description='Mirnov coils')
    parser.add_argument('-p', '--pulse',
                        help='pulse (shot) number', default='46241', type=int)
    #parser.add_argument('-s', '--shot',
    #                    type=int, help='Mds+ pulse Number ([1, ...])',
    #                    default=100)
    parser.add_argument('-t', '--ploT',
                        action='store_true', help='Plot Signals')
    parser.add_argument('-n', '--names',
                        action='store_true', help='Print Node Table')
    return parser.parse_args()


if (__name__ == "__main__"):
    args = get_arguments()
    pulseNo = args.pulse
    #  mdsPulseNumber = args.shotpulseNo
    nodeTable = build_import_table()
    if args.names:
        print('Probe \t SDAS \t MDS+')
        for nd in nodeTable:
            print(nd['name'])
            print(f"\t {nd['sdas']}")
            print(f"\t {nd['mds']}")

    if args.ploT:
        plot_signals(pulseNo, nodeTable)
    else:
        mds_writer(pulseNo, nodeTable)
