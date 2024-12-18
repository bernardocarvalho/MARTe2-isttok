#!/bin/bash
#
PATH=$PATH:$EPICS_BASE/bin/$EPICS_HOST_ARCH
caget ISTTOK:central:MARTe2-Status
caput ISTTOK:central:MARTe2-Command 1
sleep 0.5
caget ISTTOK:central:MARTe2-Status
caput ISTTOK:central:MARTe2-Command 2
sleep 3.0
caget ISTTOK:central:MARTe2-Status
caput ISTTOK:central:MARTe2-Command 0
caget ISTTOK:central:MARTe2-Status
# sleep 0.03
# caget MARTE2-DEMO-APP:STATUS
# caput MARTE2-DEMO-APP:COMMAND 2
