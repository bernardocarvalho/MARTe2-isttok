# MARTe2-isttok
Repository containing MARTe2 Components for ISTTOK RT Control

## Setting up environment

It is assumed that a clean Debian 12 is being used (https://www.debian.org)
Alternatively an CentOS/AlmaLinux 9 installation can being used (https://almalinux.org/get-almalinux).

See instructions in (https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-demos-padova)

### Download all the needed software

Open a terminal,  
`sudo apt update`  
`sudo apt upgrade`

Install all the standard development tools, the cmake compiler:

`sudo apt-get install -y build-essential git cmake`  
`sudo apt-get install -y libncurses-dev`

Solve dependencies for MARTe2 and EPICS:  
`sudo apt-get install -y libreadline-dev re2c`

Install MDSplus

See (https://www.mdsplus.org/index.php/Latest_Ubuntu/Debian_Packages)

`sudo apt-get install -y install mdsplus-kernel* mdsplus-java* mdsplus-python* mdsplus-devel*`

Create a folder (e.g. named xxx in home) and clone MARTe2 Core,  MARTe2 components and the MARTe2 isttok repo:

`mkdir ~/xxx`  
`cd ~/xxx`  

`git clone https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2.git MARTe2-dev`  
`git clone https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components.git`  
`git clone https://github.com/bernardocarvalho/MARTe2-isttok`  
      
Download EPICS R7:

`git clone --recursive https://github.com/epics-base/epics-base.git`

### Building libraries and frameworks

Compile EPICS:

`cd ~/xxx/epics-base`  
`echo "OP_SYS_CXXFLAGS += -std=c++11" >> configure/os/CONFIG_SITE.linux-x86_64.Common`  
`make`

## Compiling MARTe, EPICS, and the examples

Make sure that all the environment variables are correctly exported.

`export MARTe2_DIR=~/xxx/MARTe2-dev`

`export MARTe2_Components_DIR=~/xxx/MARTe2-components`

`export EPICS_BASE=~/xxx/epics-base`

`export EPICSPVA=~/xxx/epics-base`

`export EPICS_HOST_ARCH=linux-x86_64`

`export MDSPLUS_DIR=/usr/local/mdsplus`

`export PATH=$MDSPLUS_DIR/bin:$EPICS_BASE/bin/$EPICS_HOST_ARCH:$PATH`

`export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MARTe2_DIR/Build/x86-linux/Core/:$EPICS_BASE/lib/$EPICS_HOST_ARCH`

`cd ~/xxx/MARTe2-dev`

`make -f Makefile.linux`

`cd ~/xxx/MARTe2-components`

`make -f Makefile.linux`

`cd ~/xxx/MARTe2-isttok`

`make -C DataSources/AtcaIop -f Makefile.gcc`

## Running the ISTTOK ATCA-IOP RT App

### Start the EPICS IOC

Open a new terminal shell and start the [EPICS](https://epics-controls.org) IOC server:

`cd ~/xxx/MARTe2-isttok`  
`softIoc -d Configurations/EPICS-isttok.db`

### Start MARTe2 

Open a new terminal shell and launch the MARTe2 application

`cd ~/xxx/MARTe2-isttok/Startup`

Startup MARTe2 RT App  
`./AtcaIop.sh -l RealTimeLoader -f ../Configurations/RTApp-AtcaIop.cfg -m StateMachine:START`

   Would should see a bunch of logger messaged. Last list should be:

   ´´´
   [Warning - Threads.cpp:173]: Requested a thread priority that is higher than the one supported by the selected policy - clipping to the maximum value supported by the policy.
   [Warning - Threads.cpp:173]: Requested a thread priority that is higher than the one supported by the selected policy - clipping to the maximum value supported by the policy.
   [Warning - Threads.cpp:185]: Failed to change the thread priority (likely due to insufficient permissions)
   [Information - StateMachine.cpp:340]: In state (INITIAL) triggered message (StartNextStateExecutionMsg)

   ´´´

Alternative configuration files can also be used: ../Configurations/RTApp-ADCSimul.cfg
    
### Start open browser and explore MARTe2 App Objects

[MARTe2 HTTP local server ](http://localhost:8084)


### Change RT App State to RUN  (to store Data), and get back to IDLE

Open a new terminal shell and run:  
```console
$ caput ISTTOK:central:MARTe2-Command 1
(wait)
$ caput ISTTOK:central:MARTe2-Command 0
```


### [Isolate](https://docs.redhat.com/en/documentation/red_hat_enterprise_linux_for_real_time/7/html/tuning_guide/isolating_cpus_using_tuned-profiles-realtime) CPUs 

Install tuned, tuned-profiles-cpu-partitioning, tuned-utils packages

`numactl --hardware`

In file `/etc/tuned/cpu-partitioning-variables.conf`,  set:
isolated_cores=6-11
no_balance_cores=6-11

As root:
```bash
# tuned-adm profile cpu-partitioning
# reboot
...
# tuned-adm profile_info 

```


