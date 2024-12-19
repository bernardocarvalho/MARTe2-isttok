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

from MdsImportSdas import mds_writer, plot_signals

mdsTreeName = 'isttoksdas'


def build_import_table_mag():
    MDS_NODEFMT = "\\TOP.HARDWARE.ATCA1.IOC_0.CHANNEL_{}.RAW"
    SDAS_NODEFMT = 'MARTE_NODE_IVO3.DataCollection.Channel_{}'
    NUM_PROBES = 12

    table = []
    for n in range(NUM_PROBES):
        sdas_str = SDAS_NODEFMT.format(str(166 + n).zfill(3))
        mds_str = MDS_NODEFMT.format(str(n))
        nd = {'sdas': sdas_str, 'mds': mds_str,
              'name': f'Mirnov coil no. {n+1}'}
        table.append(nd)
    return table


def build_import_table_langmuir():
    SDAS_NODEFMT = 'MARTE_NODE_IVO3.DataCollection.Channel_{}'
    LANGMUIR_OFF = 24
    MDS_NODEFMT = "\\TOP.HARDWARE.ATCA1.IOC_1.CHANNEL_{}.RAW"
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
        mds_str = MDS_NODEFMT.format(str(n))
        nd = {'sdas': sdas_str, 'mds': mds_str,
              'name': f'Langmuir Probe no. {n+1}'}
        table.append(nd)
    return table


def get_arguments():
    parser = argparse.ArgumentParser(description='Mirnov coils')
    parser.add_argument('-p', '--pulse',
                        help='pulse (shot) number', default='46241', type=int)
    # parser.add_argument('-s', '--shot',
    #                    type=int, help='Mds+ pulse Number ([1, ...])',
    #                    default=100)
    parser.add_argument('-e', '--exportData',
                        action='store_true', help='Export to MDSPlus')
    parser.add_argument('-m', '--mirnov',
                        action='store_true', help='Import Mirnov')
    parser.add_argument('-l', '--langmuir',
                        action='store_true', help='Import Langmuir')
    parser.add_argument('-t', '--ploT',
                        action='store_true', help='Plot Signals')
    parser.add_argument('-n', '--names',
                        action='store_true', help='Print Node Table')
    return parser.parse_args()


def mds_import(args, pulseNo, nodeTable):
    if args.exportData:
        mds_writer(pulseNo, nodeTable)
    if args.ploT:
        plot_signals(pulseNo, nodeTable)
    if args.names:
        print('Probe \t SDAS \t MDS+')
        for nd in nodeTable:
            print(nd['name'])
            print(f"\t {nd['sdas']}")
            print(f"\t {nd['mds']}")


if (__name__ == "__main__"):
    args = get_arguments()
    pulseNo = args.pulse

    if args.mirnov:
        print('Getting Mirnov Probe signals')
        nodeTable = build_import_table_mag()
        mds_import(args, pulseNo, nodeTable)
    if args.langmuir:
        print('Getting Langmuir Probe signals')
        nodeTable = build_import_table_langmuir()
        mds_import(args, pulseNo, nodeTable)
