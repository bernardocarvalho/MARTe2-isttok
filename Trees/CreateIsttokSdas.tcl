edit isttoksdas/new

add node .INP
add node .REFS
add node .ERRORS
add node .CONTROLS
add node .STATES
add node .PERF
add node TIME/usage=signal
add node TIMED/usage=signal

add node .HARDWARE
set def  .HARDWARE
add node .ATCA1
add node .ATCA2
# set def \ISTTOKSDAS::TOP.HARDWARE.ATCA1
set def .ATCA1
add node IOC_0/model=IOCMIMORT
add node IOC_1/model=IOCMIMORT
set def \ISTTOKSDAS::TOP.HARDWARE.ATCA1.IOC_1"
put SLOT "2"
# put CLOCK_FREQ "20000"


set def \ISTTOKSDAS::TOP.INP
add node MEAS0/usage=signal
add node MEAS0D/usage=signal
add node MEAS0F/usage=signal
add node MEAS0FD/usage=signal
add node MEAS1/usage=signal
add node MEAS1D/usage=signal
add node MEAS1F/usage=signal
add node MEAS1FD/usage=signal

set def \ISTTOKSDAS::TOP.REFS
add node REF0/usage=signal
add node REF0D/usage=signal
add node REF1/usage=signal
add node REF1D/usage=signal

set def \ISTTOKSDAS::TOP.ERRORS
add node ERROR0/usage=signal
add node ERROR0D/usage=signal
add node ERROR1/usage=signal
add node ERROR1D/usage=signal

set def \ISTTOKSDAS::TOP.CONTROLS
add node CONTROL0/usage=signal
add node CONTROL0D/usage=signal
add node CONTROL1/usage=signal
add node CONTROL1D/usage=signal

set def \ISTTOKSDAS::TOP.STATES
add node STATE0/usage=signal
add node STATE0D/usage=signal
add node STATE1/usage=signal
add node STATE1D/usage=signal
add node STATE2/usage=signal
add node STATE2D/usage=signal
add node STATE3/usage=signal
add node STATE3D/usage=signal

set def \ISTTOKSDAS::TOP.PERF
add node CYCLET/usage=signal
add node CYCLETD/usage=signal

write
close
# set tree isttoksdas
# create pulse 100
exit
