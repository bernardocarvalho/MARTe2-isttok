edit isttokoutput/new

add node .INP
add node .OUT
add node .REFS
add node .ERRORS
add node .CONTROLS
add node .STATES
add node .PERF
add node TIME/usage=signal
add node TIMED/usage=signal

#set def \ISTTOKSDAS::TOP.HARDWARE.ATCA1.IOC1.CHANNELS
#add node .INPUT_11
#set def  .INPUT_11
#add node RAW/usage=signal
#add node GAIN/usage=numeric
#put GAIN "1.0"
#add node OFFSET/usage=numeric
#put OFFSET "0.0"
#add node SIGNAL/usage=signal
#put SIGNAL "BUILD_SIGNAL(GAIN*(RAW - OFFSET),RAW)"


set def \ISTTOKOUTPUT::TOP.OUT
add node OUT0/usage=signal
add node OUT1/usage=signal
add node OUT2/usage=signal
add node OUT3/usage=signal
add node OUT4/usage=signal
add node OUT5/usage=signal
add node OUT6/usage=signal
add node OUT7/usage=signal
add node CYCLE_TIME/usage=signal
add node IPLASMA/usage=signal
add node IP_X/usage=signal
add node IP_Y/usage=signal

set def \ISTTOKOUTPUT::TOP.INP
add node MEAS0/usage=signal
add node MEAS0D/usage=signal
add node MEAS0F/usage=signal
add node MEAS0FD/usage=signal
add node MEAS1/usage=signal
add node MEAS1D/usage=signal
add node MEAS1F/usage=signal
add node MEAS1FD/usage=signal

set def \ISTTOKOUTPUT::TOP.REFS
add node REF0/usage=signal
add node REF0D/usage=signal
add node REF1/usage=signal
add node REF1D/usage=signal

set def \ISTTOKOUTPUT::TOP.ERRORS
add node ERROR0/usage=signal
add node ERROR0D/usage=signal
add node ERROR1/usage=signal
add node ERROR1D/usage=signal

set def \ISTTOKOUTPUT::TOP.CONTROLS
add node CONTROL0/usage=signal
add node CONTROL0D/usage=signal
add node CONTROL1/usage=signal
add node CONTROL1D/usage=signal

set def \ISTTOKOUTPUT::TOP.STATES
add node STATE0/usage=signal
add node STATE0D/usage=signal
add node STATE1/usage=signal
add node STATE1D/usage=signal
add node STATE2/usage=signal
add node STATE2D/usage=signal
add node STATE3/usage=signal
add node STATE3D/usage=signal

set def \ISTTOKOUTPUT::TOP.PERF
add node CYCLET/usage=signal
add node CYCLETD/usage=signal

write
close
set tree isttokoutput
create pulse 1
exit
