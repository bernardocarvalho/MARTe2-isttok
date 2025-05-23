#!/usr/bin/env python3
"""
This script update a MDSplus Tree with the epics EO/WO offsets

First use:
 export EPICS_CA_AUTO_ADDR_LIST="NO"
 export EPICS_CA_ADDR_LIST="192.168.1.110"
"""
import numpy as np

import mdsthin
from epics import caget
import argparse

ADC_CHANNELS = 12  # channels stored in ISTTOK
MDSTREENAME = 'isttokmarte'

EOPV = 'ISTTOK:central:ATCAIOP1-EO'
EO_NODE = '\\TOP.HARDWARE.ATCA_2.IOP_9.CHANNEL_{}.EO_OFFSET'
WOPV = 'ISTTOK:central:ATCAIOP1-WO'
WO_NODE = '\\TOP.HARDWARE.ATCA_2.IOP_9.CHANNEL_{}.WO_OFFSET'
# Connect over SSH
c = mdsthin.Connection('ssh://oper@atca-marte2')


def saveEoWo(args):
    mdsPulseNumber = args.shot
    try:
        if (mdsPulseNumber > 0):
            c.openTree(MDSTREENAME, mdsPulseNumber)
        else:
            # Open the current shot
            c.openTree(MDSTREENAME, 0)

    except Exception:
        print(f'Failed opening {MDSTREENAME} for pulse number ' +
              f'{mdsPulseNumber:d}')
        exit()
    print(f'Opening Tree {MDSTREENAME} for pulse {mdsPulseNumber:d}')
    eo = caget(EOPV)
    wo = caget(WOPV)
    for i, e in enumerate(eo):
        # Read individual nodes
        y = c.get(EO_NODE.format(i)).data()
        print(f" i: {i}, {e}, m: {y}")
        # Using an explicit MDSplus type
        c.put(EO_NODE.format(i), '$', mdsthin.Int32(e))

    for i, w in enumerate(wo):
        y = c.get(WO_NODE.format(i)).data()
        print(f" i: {i}, {w}, m: {y}")
        c.put(WO_NODE.format(i), '$', mdsthin.Float32(w))
    # print(wo)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(
            description='Script to update a MDSplus Tree with the epics EO/WO')

    parser.add_argument('-s', '--shot',
                        type=int, help='Mds+ pulse Number ([1, ...])',
                        default=52739)
    # parser.add_argument('-e', '--averages', action='store_true', help='Calc averages')
# parser.add_argument('-w', '--drift', action='store_true', help='Calc drifts')

    args = parser.parse_args()
    saveEoWo(args)
