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
import mdsthin
c = mdsthin.Connection('localhost')
c.get('whoami()').data()
c.openTree('isttokoutput', 1)

