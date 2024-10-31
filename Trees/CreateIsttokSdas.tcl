edit isttoksdas/new

#add node .ATCAIOP1
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
add node .IOC1
set def .IOC1
add node COMMENT/usage=text
add node SLOT/usage=numeric
add node CLOCK_FREQ/usage=numeric
put CLOCK_FREQ "20000"

add node .CHANNELS
set def .CHANNELS
add node .INPUT_00
set def  .INPUT_00
add node RAW/usage=signal
add node GAIN/usage=numeric
# put GAIN "0.000076294"
put GAIN "1.0"
add node OFFSET/usage=numeric
put OFFSET "0.0"
add node SIGNAL/usage=signal
put SIGNAL "BUILD_SIGNAL(GAIN*(RAW - OFFSET),RAW)"

set def \ISTTOKSDAS::TOP.HARDWARE.ATCA1.IOC1.CHANNELS
add node .INPUT_01
set def  .INPUT_01
add node RAW/usage=signal
add node GAIN/usage=numeric
put GAIN "1.0"
add node OFFSET/usage=numeric
put OFFSET "0.0"
add node SIGNAL/usage=signal
put SIGNAL "BUILD_SIGNAL(GAIN*(RAW - OFFSET),RAW)"

set def \ISTTOKSDAS::TOP.HARDWARE.ATCA1.IOC1.CHANNELS
add node .INPUT_02
set def  .INPUT_02
add node RAW/usage=signal
add node GAIN/usage=numeric
put GAIN "1.0"
add node OFFSET/usage=numeric
put OFFSET "0.0"
add node SIGNAL/usage=signal
put SIGNAL "BUILD_SIGNAL(GAIN*(RAW - OFFSET),RAW)"

set def \ISTTOKSDAS::TOP.HARDWARE.ATCA1.IOC1.CHANNELS
add node .INPUT_03
set def  .INPUT_03
add node RAW/usage=signal
add node GAIN/usage=numeric
put GAIN "1.0"
add node OFFSET/usage=numeric
put OFFSET "0.0"
add node SIGNAL/usage=signal
put SIGNAL "BUILD_SIGNAL(GAIN*(RAW - OFFSET),RAW)"

set def \ISTTOKSDAS::TOP.HARDWARE.ATCA1.IOC1.CHANNELS
add node .INPUT_04
set def  .INPUT_04
add node RAW/usage=signal
add node GAIN/usage=numeric
put GAIN "1.0"
add node OFFSET/usage=numeric
put OFFSET "0.0"
add node SIGNAL/usage=signal
put SIGNAL "BUILD_SIGNAL(GAIN*(RAW - OFFSET),RAW)"

set def \ISTTOKSDAS::TOP.HARDWARE.ATCA1.IOC1.CHANNELS
add node .INPUT_05
set def  .INPUT_05
add node RAW/usage=signal
add node GAIN/usage=numeric
put GAIN "1.0"
add node OFFSET/usage=numeric
put OFFSET "0.0"
add node SIGNAL/usage=signal
put SIGNAL "BUILD_SIGNAL(GAIN*(RAW - OFFSET),RAW)"

set def \ISTTOKSDAS::TOP.HARDWARE.ATCA1.IOC1.CHANNELS
add node .INPUT_06
set def  .INPUT_06
add node RAW/usage=signal
add node GAIN/usage=numeric
put GAIN "1.0"
add node OFFSET/usage=numeric
put OFFSET "0.0"
add node SIGNAL/usage=signal
put SIGNAL "BUILD_SIGNAL(GAIN*(RAW - OFFSET),RAW)"

set def \ISTTOKSDAS::TOP.HARDWARE.ATCA1.IOC1.CHANNELS
add node .INPUT_07
set def  .INPUT_07
add node RAW/usage=signal
add node GAIN/usage=numeric
put GAIN "1.0"
add node OFFSET/usage=numeric
put OFFSET "0.0"
add node SIGNAL/usage=signal
put SIGNAL "BUILD_SIGNAL(GAIN*(RAW - OFFSET),RAW)"

set def \ISTTOKSDAS::TOP.HARDWARE.ATCA1.IOC1.CHANNELS
add node .INPUT_08
set def  .INPUT_08
add node RAW/usage=signal
add node GAIN/usage=numeric
put GAIN "1.0"
add node OFFSET/usage=numeric
put OFFSET "0.0"
add node SIGNAL/usage=signal
put SIGNAL "BUILD_SIGNAL(GAIN*(RAW - OFFSET),RAW)"

set def \ISTTOKSDAS::TOP.HARDWARE.ATCA1.IOC1.CHANNELS
add node .INPUT_09
set def  .INPUT_09
add node RAW/usage=signal
add node GAIN/usage=numeric
put GAIN "1.0"
add node OFFSET/usage=numeric
put OFFSET "0.0"
add node SIGNAL/usage=signal
put SIGNAL "BUILD_SIGNAL(GAIN*(RAW - OFFSET),RAW)"

set def \ISTTOKSDAS::TOP.HARDWARE.ATCA1.IOC1.CHANNELS
add node .INPUT_10
set def  .INPUT_10
add node RAW/usage=signal
add node GAIN/usage=numeric
put GAIN "1.0"
add node OFFSET/usage=numeric
put OFFSET "0.0"
add node SIGNAL/usage=signal
put SIGNAL "BUILD_SIGNAL(GAIN*(RAW - OFFSET),RAW)"

set def \ISTTOKSDAS::TOP.HARDWARE.ATCA1.IOC1.CHANNELS
add node .INPUT_11
set def  .INPUT_11
add node RAW/usage=signal
add node GAIN/usage=numeric
put GAIN "1.0"
add node OFFSET/usage=numeric
put OFFSET "0.0"
add node SIGNAL/usage=signal
put SIGNAL "BUILD_SIGNAL(GAIN*(RAW - OFFSET),RAW)"


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
