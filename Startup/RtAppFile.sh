#!/bin/bash
#Arguments -f FILENAME -m MESSAGE | -s STATE [-d cgdb|strace]

MDS=0
DEBUG=""
INPUT_ARGS=$*
while test $# -gt 0
do
    case "$1" in
        -d|--debug)
        DEBUG="$2"
        ;;
        -mds)
        MDS=1
        ;;
    esac
    shift
done

MARTe2_HOME=~/git-repos/MARTe2

MARTe2_DIR=$MARTe2_HOME/MARTe2
MARTe2_Components_DIR=$MARTe2_HOME/MARTe2-components
MARTe2_Demos_DIR=~/git-repos/MARTe2-demos-padova

EPICS_ROOT=/opt/epics
EPICS_BASE=${EPICS_ROOT}/epics-base
EPICS_HOST_ARCH=linux-x86_64
EPICS_CA_ADDR_LIST="localhost"
#export EPICS_CA_ADDR_LIST="192.168.0.60 192.168.0.21 192.168.0.22 192.168.0.99"
#export EPICS_CA_ADDR_LIST="192.168.88.251"
# export EPICS_CA_ADDR_LIST="10.10.136.177"
export EPICS_CA_AUTO_ADDR_LIST="NO"
# export rtappdemo_path=$MARTe2_Demos_DIR/Trees
# rtappreader_path=$MARTe2_Demos_DIR/Trees

LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../Build/x86-linux/Components/GAMs/ElectricProbesGAM/

# LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../Build/x86-linux/Components/DataSources/ADCSimulator/
#LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../Build/x86-linux/Components/Other/ControllerEx1/
#LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../Build/x86-linux/Components/Other/ControllerEx2/

LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_DIR/Build/x86-linux/Core/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/DataSources/EPICSCA/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/DataSources/EPICSPVA/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/DataSources/LinuxTimer/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/DataSources/LoggerDataSource/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/DataSources/FileDataSource/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/DataSources/LinkDataSource/
#LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/DataSources/UDP/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/DataSources/MDSReader/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/DataSources/MDSWriter/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/DataSources/RealTimeThreadSynchronisation/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/DataSources/RealTimeThreadAsyncBridge/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/GAMs/ConstantGAM/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/GAMs/ConversionGAM/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/GAMs/FlattenedStructIOGAM/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/GAMs/IOGAM/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/GAMs/MessageGAM/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/GAMs/FilterGAM/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/GAMs/HistogramGAM/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/GAMs/SSMGAM/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/GAMs/TriggerOnChangeGAM/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/GAMs/WaveformGAM/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/Interfaces/BaseLib2Wrapper/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/Interfaces/EPICS/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/Interfaces/EPICSPVA/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/Interfaces/MemoryGate/
# LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/Interfaces/NI9157Device/
# LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/Interfaces/OPCUA/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_Components_DIR/Build/x86-linux/Components/Interfaces/SysLogger/

LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$EPICS_BASE/lib/$EPICS_HOST_ARCH

echo $LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH

#export isttoksdas_path=/home/mdsplus/Trees
export isttokoutput_path=/home/mdsplus/Trees

# export rtappdemo_path=../Trees
#if [ ${MDS} == 1 ]; then
#fi

if [ "$DEBUG" = "cgdb" ]
then
    cgdb --args $MARTe2_DIR/Build/x86-linux/App/MARTeApp.ex $INPUT_ARGS
else
    $MARTe2_DIR/Build/x86-linux/App/MARTeApp.ex $INPUT_ARGS
fi

