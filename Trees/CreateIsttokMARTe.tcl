edit isttokmarte/new

add node .INP
add node .REFS
add node .ERRORS
add node .CONTROLS
add node .STATES
add node .PERF
add node TIME/usage=signal
add node TIMED/usage=signal

add node .DIAGNOSTICS
add node .HARDWARE
set def  .HARDWARE
add node .ATCA_1
add node .ATCA_2
# set def \ISTTOKSDAS::TOP.HARDWARE.ATCA1
set def .ATCA_1
add node IOC_0/model=IOCMIMORT
add node IOC_1/model=IOCMIMORT
set def \ISTTOKMARTE::TOP.HARDWARE.ATCA_1.IOC_1"
put SLOT "2"
# put CLOCK_FREQ "20000"

set def \ISTTOKMARTE::TOP.HARDWARE.ATCA_2"
add node IOP_9/model=ATCA_IOP
set def \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9"
put RT_DECIM "200"
put SLOT "9"
set def \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.CHANNEL_0
# calibration on 14/05/2025
put gain "1.73887463e-05"
put inp_decim "GAIN * ADC_DECIM"
put inp_integ "GAIN * ADC_INTEG / \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.RT_DECIM / \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.CLOCK_FREQ"
set def \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.CHANNEL_1
put gain "1.73937683e-05"
put inp_decim "GAIN * ADC_DECIM"
put inp_integ "GAIN * ADC_INTEG / \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.RT_DECIM / \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.CLOCK_FREQ"
set def \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.CHANNEL_2
put gain "1.74275762e-05"
put inp_decim "GAIN * ADC_DECIM"
put inp_integ "GAIN * ADC_INTEG / \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.RT_DECIM / \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.CLOCK_FREQ"
set def \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.CHANNEL_3
put gain "1.74994244e-05"
put inp_decim "GAIN * ADC_DECIM"
put inp_integ "GAIN * ADC_INTEG / \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.RT_DECIM / \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.CLOCK_FREQ"
set def \isttokmarte::top.hardware.atca_2.iop_9.channel_4
put gain "1.73826859e-05"
put inp_decim "GAIN * ADC_DECIM"
put inp_integ "GAIN * ADC_INTEG / \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.RT_DECIM / \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.CLOCK_FREQ"
set def \isttokmarte::top.hardware.atca_2.iop_9.channel_5
put gain "1.74439575e-05"
put inp_decim "GAIN * ADC_DECIM"
put inp_integ "GAIN * ADC_INTEG / \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.RT_DECIM / \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.CLOCK_FREQ"
set def \isttokmarte::top.hardware.atca_2.iop_9.channel_6
put gain "1.74532093e-05"
put inp_decim "GAIN * ADC_DECIM"
put inp_integ "GAIN * ADC_INTEG / \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.RT_DECIM / \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.CLOCK_FREQ"
set def \isttokmarte::top.hardware.atca_2.iop_9.channel_7
put gain "1.73723740e-05"
put inp_decim "GAIN * ADC_DECIM"
put inp_integ "GAIN * ADC_INTEG / \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.RT_DECIM / \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.CLOCK_FREQ"
set def \isttokmarte::top.hardware.atca_2.iop_9.channel_8
put gain "1.71925327e-05"
put inp_decim "GAIN * ADC_DECIM"
put inp_integ "GAIN * ADC_INTEG / \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.RT_DECIM / \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.CLOCK_FREQ"
set def \isttokmarte::top.hardware.atca_2.iop_9.channel_9
put gain "1.72739424e-05"
put inp_decim "GAIN * ADC_DECIM"
put inp_integ "GAIN * ADC_INTEG / \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.RT_DECIM / \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.CLOCK_FREQ"
set def \isttokmarte::top.hardware.atca_2.iop_9.channel_10
put gain "1.73809822e-05"
put inp_decim "GAIN * ADC_DECIM"
put inp_integ "GAIN * ADC_INTEG / \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.RT_DECIM / \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.CLOCK_FREQ"
set def \isttokmarte::top.hardware.atca_2.iop_9.channel_11
put gain "1.74412886e-05"
put inp_decim "GAIN * ADC_DECIM"
put inp_integ "GAIN * ADC_INTEG / \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.RT_DECIM / \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.CLOCK_FREQ"

#set def \ISTTOKMARTE::TOP.INP
#add node MEAS0/usage=signal
#add node MEAS0D/usage=signal
#add node MEAS0F/usage=signal
#add node MEAS0FD/usage=signal
#add node MEAS1/usage=signal
#add node MEAS1D/usage=signal
#add node MEAS1F/usage=signal
#add node MEAS1FD/usage=signal

set def \ISTTOKMARTE::TOP.REFS
add node REF0/usage=signal
add node REF0D/usage=signal
add node REF1/usage=signal
add node REF1D/usage=signal

set def \ISTTOKMARTE::TOP.ERRORS
add node ERROR0/usage=signal
add node ERROR0D/usage=signal
add node ERROR1/usage=signal
add node ERROR1D/usage=signal

set def \ISTTOKMARTE::TOP.CONTROLS
add node CONTROL0/usage=signal
add node CONTROL0D/usage=signal
add node CONTROL1/usage=signal
add node CONTROL1D/usage=signal

set def \ISTTOKMARTE::TOP.STATES
add node STATE0/usage=signal
add node STATE0D/usage=signal
add node STATE1/usage=signal
add node STATE1D/usage=signal
add node STATE2/usage=signal
add node STATE2D/usage=signal
add node STATE3/usage=signal
add node STATE3D/usage=signal

set def \ISTTOKMARTE::TOP.PERF
add node CYCLET/usage=signal
add node CYCLETD/usage=signal

set def \ISTTOKMARTE::TOP.DIAGNOSTICS
add node MAGNETICS
set def .MAGNETICS
add node .MIRNOV
set def .MIRNOV
add node PROBE1
add node PROBE2
add node PROBE3
add node PROBE4
add node PROBE5
add node PROBE6
add node PROBE7
add node PROBE8
add node PROBE9
add node PROBE10
add node PROBE11
add node PROBE12
set def \ISTTOKMARTE::TOP.DIAGNOSTICS.MAGNETICS.MIRNOV.PROBE1
add node ANGLE/usage=numeric
put ANGLE "15.1"
add node AREA/usage=numeric
# Total area: turns * A : 0.0049**2 * 50
put AREA "0.0012005"
add node POL/usage=numeric
put POL "-1"
add node SIG/usage=signal
put SIG "POL * \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.CHANNEL_0.INP_INTEG"
set def \ISTTOKMARTE::TOP.DIAGNOSTICS.MAGNETICS.MIRNOV.PROBE2
add node ANGLE/usage=numeric
put ANGLE "30.2"
add node AREA/usage=numeric
put AREA "0.0012005"
add node POL/usage=numeric
put POL "-1"
add node SIG/usage=signal
put SIG "POL * \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.CHANNEL_1.INP_INTEG"
set def \ISTTOKMARTE::TOP.DIAGNOSTICS.MAGNETICS.MIRNOV.PROBE3
add node ANGLE/usage=numeric
put ANGLE "30.2"
add node AREA/usage=numeric
put AREA "0.0012005"
add node POL/usage=numeric
put POL "1"
add node SIG/usage=signal
put SIG "POL * \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.CHANNEL_2.INP_INTEG"
set def \ISTTOKMARTE::TOP.DIAGNOSTICS.MAGNETICS.MIRNOV.PROBE4
add node ANGLE/usage=numeric
put ANGLE "30.2"
add node AREA/usage=numeric
put AREA "0.0012005"
add node POL/usage=numeric
put POL "-1"
add node SIG/usage=signal
put SIG "POL * \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.CHANNEL_3.INP_INTEG"

set def \ISTTOKMARTE::TOP.DIAGNOSTICS.MAGNETICS.MIRNOV.PROBE5
add node ANGLE/usage=numeric
put ANGLE "30.2"
add node AREA/usage=numeric
put AREA "0.0012005"
add node POL/usage=numeric
put POL "1"
add node SIG/usage=signal
put SIG "POL * \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.CHANNEL_4.INP_INTEG"
set def \ISTTOKMARTE::TOP.DIAGNOSTICS.MAGNETICS.MIRNOV.PROBE6
add node ANGLE/usage=numeric
put ANGLE "30.2"
add node AREA/usage=numeric
put AREA "0.0012005"
add node POL/usage=numeric
put POL "1"
add node SIG/usage=signal
put SIG "POL * \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.CHANNEL_5.INP_INTEG"

set def \ISTTOKMARTE::TOP.DIAGNOSTICS.MAGNETICS.MIRNOV.PROBE7
add node ANGLE/usage=numeric
put ANGLE "30.2"
add node AREA/usage=numeric
put AREA "0.0012005"
add node POL/usage=numeric
put POL "1"
add node SIG/usage=signal
put SIG "POL * \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.CHANNEL_6.INP_INTEG"
set def \ISTTOKMARTE::TOP.DIAGNOSTICS.MAGNETICS.MIRNOV.PROBE8
add node ANGLE/usage=numeric
put ANGLE "30.2"
add node AREA/usage=numeric
put AREA "0.0012005"
add node POL/usage=numeric
put POL "1"
add node SIG/usage=signal
put SIG "POL * \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.CHANNEL_7.INP_INTEG"

set def \ISTTOKMARTE::TOP.DIAGNOSTICS.MAGNETICS.MIRNOV.PROBE9
add node ANGLE/usage=numeric
put ANGLE "30.2"
add node AREA/usage=numeric
put AREA "0.0012005"
add node POL/usage=numeric
put POL "1"
add node SIG/usage=signal
put SIG "POL * \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.CHANNEL_8.INP_INTEG"
set def \ISTTOKMARTE::TOP.DIAGNOSTICS.MAGNETICS.MIRNOV.PROBE10
add node ANGLE/usage=numeric
put ANGLE "30.2"
add node AREA/usage=numeric
put AREA "0.0012005"
add node POL/usage=numeric
put POL "1"
add node SIG/usage=signal
put SIG "POL * \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.CHANNEL_9.INP_INTEG"

set def \ISTTOKMARTE::TOP.DIAGNOSTICS.MAGNETICS.MIRNOV.PROBE11
add node ANGLE/usage=numeric
put ANGLE "30.2"
add node AREA/usage=numeric
put AREA "0.0012005"
add node POL/usage=numeric
put POL "-1"
add node SIG/usage=signal
put SIG "POL * \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.CHANNEL_10.INP_INTEG"
set def \ISTTOKMARTE::TOP.DIAGNOSTICS.MAGNETICS.MIRNOV.PROBE12
add node ANGLE/usage=numeric
put ANGLE "30.2"
add node AREA/usage=numeric
put AREA "0.0012005"
add node POL/usage=numeric
put POL "1"
add node SIG/usage=signal
put SIG "POL * \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.CHANNEL_11.INP_INTEG"


write
close
# set tree isttokmarte
# create pulse 100
exit
# edit isttokmarte/shot=-1
#TCL> set def \ISTTOKMARTE::TOP.HARDWARE.ATCA_2.IOP_9.CHANNEL_0
# TCL> put INP_DECIM "GAIN * ADC_DECIM"
#TCL> put INP_INTEG "GAIN * ADC_INTEG / .HARDWARE.ATCA_2.IOP_9.RT_DECIM / .HARDWARE.ATCA_2.IOP_9.CLOCK_FREQ"

# TCL> write
