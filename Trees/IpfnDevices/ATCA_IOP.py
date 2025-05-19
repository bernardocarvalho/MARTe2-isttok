from MDSplus import Device, Data, Range, Dimension, Window, Signal, Int16Array

NUM_CHANNELS = 16

class ATCA_IOP(Device):
    """
    ATCA IPFN IOP Digitizer, 16 channel, 18 bit digitizer, Chopper, Integrator

 This class inherits from the generic Device class
 Class Device provides the basic methods required by MDSplus, in particular the Add metod
 called when a new instance of this device has to be created in a tree in editing
 it is therefore only necessary to add here the device specific methods such as init and store
 It is however necessary to define the structure of the device, so that the superclass' Add method can
 build the appropiate device subtree. This information is specified as a list of dictionaries
 every element of the list specifies a node of the subtree associated with the device. The mandatory dictionary items
# for each node are:
#  - path : the path name relative to the subtree root of the node
#  - type : the type (usage) of the node, which can be either 'text', 'numeric', 'signal', 'action', 'structure'
# optional dictionary items formeach node are:
#  - value : initial value forn that node
#  - valueExpr : initial value when specified as an expression
#  - options : a (list of) NCI options for the tree node, such as 'no_write_model', 'no_write_shot', 'compress_on_put'
# field parts will contain the list of dictionaries, and will be used by Device superclass.
#
# in the following methods, defice fields are referred by the syntax: self.<field_name>, where field_name is derived by the
# path of the corresponding tree node relative to the subtree (as specified by the corresponding path dictionary item), where #letters are lowercase and the dots and colons are replaced by underscores (except the first one).
# For example, tree node :ADDR is accessed by field self.addr and .CHANNEL_1:DATA by field self.channel_1_data
# All there firlds are TreeNode instances and therefore all TreeNode methods such as Data() or putData() can be used.
"""
    parts = [
        {'path': ':SLOT', 'type': 'numeric'}, {
            'path': ':COMMENT', 'type': 'text'},
        {'path': ':CLOCK_FREQ', 'type': 'numeric', 'value': 10000},
        {'path': ':RT_DECIM', 'type': 'numeric', 'value': 200},
        {'path': ':TRIG_SOURCE', 'type': 'numeric', 'value': 0},
        {'path': ':ADC_BITS', 'type': 'numeric', 'value': 18},
    ]
    
    for i in range(NUM_CHANNELS):
        parts.extend([
            {'path': '.CHANNEL_%d' % (i), 'type': 'structure'},
            #  %.5e" % (10./2**17)
            {'path': '.CHANNEL_%d:GAIN' % (
                i), 'type': 'numeric', 'value': 1.73887463e-05},
            {'path': '.CHANNEL_%d:EO_OFFSET' % (
                i), 'type': 'numeric', 'value': 0},
            {'path': '.CHANNEL_%d:WO_OFFSET' % (
                i), 'type': 'numeric', 'value': 0.0},
            {'path': '.CHANNEL_%d:START_IDX' % (
                i), 'type': 'numeric', 'value': 0},
            {'path': '.CHANNEL_%d:END_IDX' % (
                i), 'type': 'numeric', 'value': 1000},
            {'path': '.CHANNEL_%d:ADC_DECIM' % (i), 'type': 'signal'},
            {'path': '.CHANNEL_%d:ADC_DECIM_D' % (i), 'type': 'signal'},
            {'path': '.CHANNEL_%d:INP_DECIM' % (i), 'type': 'signal'},
            #       'value': "GAIN * ADC_DECIM"},
            {'path': '.CHANNEL_%d:ADC_INTEG' % (i), 'type': 'signal'},
            {'path': '.CHANNEL_%d:ADC_INTEG_D' % (i), 'type': 'signal'},
            {'path': '.CHANNEL_%d:INP_INTEG' % (i), 'type': 'signal'},
            # 'value': "GAIN * ADC_INTEG / RT_DECIM / CLOCK_FREQ"},
        ])
    parts.extend([
        {'path': ':INIT_ACTION', 'type': 'action',
         'valueExpr': "Action(Dispatch('ATCA_SERVER','INIT',50,None),Method(None,'init',head))",
         'options': ('no_write_shot',)},
        {'path': ':STORE_ACTION', 'type': 'action',
         'valueExpr': "Action(Dispatch('ATCA_SERVER','STORE',50,None),Method(None,'store',head))",
         'options': ('no_write_shot',)},
    ])
    del( i)

#            {'path': '.CHANNEL_%d:ADC_INTEG' % (i), 'type': 'signal', 'options': (
#                'no_write_model', 'compress_on_put')},
# ----------------INIT-------------------
# init method, called to configure the ADC device. It will read configuration from the corresponding subtree
# and it will download configuration to the device
    def init(self):
        # Need to import some classes from MDSplus package
        # The device will be configured via a shared library (libDemoAdc in the MDSplus distribution) defining the following routines:
        # initialize(char *addr, int clockFreq, int postTriggerSamples)
        # where clockFreq can have the following values:
        #  - 1 -> clock freq. = 1KHz
        #  - 2 -> clock freq. = 5KHz
        #  - 3 -> clock freq. = 10KHz
        #  - 4 -> clock freq. = 50 KHz
        #  - 5 -> clock freq. = 100KHz
        # trigger(char *addr)
        # acquire(char *addr, short *c1, short *c2 short *c3, short *c4)
        # the routine acquire returns 4  simulated sinusoidal waveform signals at the following frequencies:
        # Channel 1: 10Hz
        # Channel 2: 50 Hz
        # Channel 3: 100 Hz
        # Channel 4: 200Hz
        #
        # The addr argument passed to all device routines is not used and simulates the device identifier
        # used in real devices
        #

        # return success
        return 1

# -------------STORE################################################
# store method, called to get samples from the ADC and to store waveforms in the tree
    def store(self):
        # import required symbols from MDSSplus and ctypes packages

        # return success (odd numbers in MDSplus)
        return 1


"""
# in the following, we'll get data items in two steps (on the same line):
# 1) instantiate a TreeNode object, passing the integer nid to the constructor
# 2) read and evaluate (in the case the content is an expression) its content via TreeNode.data() method
# all data access operation will be but in a try block in order to check for missing or wrong configuration data
        try:
            address = self.addr.data()
# we expect to get a string in addr
        except:
            print ('Missing addr in device')
            return 0

# read the clock frequency and convert to clock mode. We use a dictionary for the conversion, and assume
        clockDict = {1000: 1, 5000: 2, 10000: 3, 50000: 4, 100000: 5}
        try:
            clockFreq = self.clock_freq.data()
            clockMode = clockDict[clockFreq]
        except:
            print ('Missing or invalid clock frequency')
            return 0

# read Post Trigger Samples and check for consistency
        try:
            pts = self.pts.data()
        except:
            print ('Missing or invalid Post Trigger Samples')
            return 0

# all required configuation collected. Call external routine initialize passing the right parameters
# we use ctypes functions to convert python variable to appropriate C types to be passed to the external routine
#        try:
        print(address)
        deviceLibCDLL.initialize(
                c_char_p(address.encode()), c_int(clockMode), c_int(pts))
        #try:
         #   deviceLibCDLL.initialize(
          #      c_char_p(address), c_int(clockMode), c_int(pts))
        #except:
         #   print('Error initializing driver')
          #  return 0
"""


