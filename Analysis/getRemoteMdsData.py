""" 
Thin client configuration
https://www.mdsplus.org/index.php?title=Documentation:Tutorial:RemoteAccess&open=101601206494234739185&page=Documentation%2FTutorials%2FRemote+RemoteAccess
from MDSplus import Connection
conn = Connection('epics.ipfn.tecnico.ulisboa.pt')
conn.openTree('rtappisttok', 131)
result = conn.get("ATCAIOP1.ADC0RAW")
result.data()
https://github.com/MDSplus/mdsthin
"""
# import mdsthin
from mdsplusClient import mdsplusClient
#from MDSplus import Connection

ADC_NODE = '\\TOP.HARDWARE.ATCA_2.IOP_9.CHANNEL_{}.ADC_DECIM'
ADC_INTEG = '\\TOP.HARDWARE.ATCA_2.IOP_9.CHANNEL_{}.ADC_INTEG'
# c = mdsthin.Connection('localhost')
client = mdsplusClient()
client.openTree()
# conn = mdsthin.Connection('ssh://epics.ipfn.tecnico.ulisboa.pt')
# conn = Connection('epics.ipfn.tecnico.ulisboa.pt')
#conn.get('whoami()').data()
# conn.openTree('isttokmarte', 51608)
channel = 1  # 0..13
data = client.getData(ADC_NODE.format(channel))
