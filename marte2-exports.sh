# MARTe2 local envs for running/compiling MARTe2 Rt Apps
export TARGET=x86-linux
export MARTe2_HOME=~/git-repos/MARTe2

export MARTe2_DIR=$MARTe2_HOME/MARTe2
export MARTe2_Components_DIR=$MARTe2_HOME/MARTe2-components
export MARTe2_Demos_DIR=~/git-repos/MARTe2-demos-padova

export EPICS_BASE=/opt/epics/epics-base
export EPICSPVA=/opt/epics/epics-base
export EPICS_HOST_ARCH=linux-x86_64

#export EPICS_CA_ADDR_LIST="localhost 192.168.1.110 efda-marte.ipfn.tecnico.ulisboa.pt"
export EPICS_CA_ADDR_LIST="localhost"
export EPICS_CA_AUTO_ADDR_LIST="NO"

export MDSPLUS_DIR=/usr/local/mdsplus
# export default_tree_path=$MARTe2_HOME/mdsplus-trees
export default_tree_path=$MARTe2_HOME/mdsplus-trees
export rtappdemo_path=~/git-repos/MARTe2-isttok/Trees
export rtappwriter_path=~/git-repos/MARTe2-isttok/Trees
# export isttoksdas_path=~/git-repos/MARTe2-isttok/Trees
export isttoksdas_path=/home/mdsplus/Trees
# export isttokoutput_path=~/git-repos/MARTe2-isttok/Trees
export isttokoutput_path=/home/mdsplus/Trees
export isttokmarte_path=/home/mdsplus/Trees
# export rtappisttok_path=$MARTe2_HOME/mdsplus-trees

export PATH=$MDSPLUS_DIR/bin:$EPICS_BASE/bin/$EPICS_HOST_ARCH:$PATH
#export SDN_CORE_INCLUDE_DIR=~/Projects/SDN_1.0.12_nonCCS/src/main/c++/include/
#export SDN_CORE_LIBRARY_DIR=~/Projects/SDN_1.0.12_nonCCS/target/lib/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_DIR/Build/x86-linux/Core/:$EPICS_BASE/lib/$EPICS_HOST_ARCH
#:$SDN_CORE_LIBRARY_DIR
source /usr/local/mdsplus/setup.sh
