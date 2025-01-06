""""
"""
import mdsthin


class mdsClient():
    """
    Models
    """

    def __init__(self, host='epics.ipfn.tecnico.ulisboa.pt', user='oper'):
        """
        Initializes the MDSPlus thin Connection
        """
        self.conn = mdsthin.Connection(f'ssh://{user}@{host}')

    def openTree(self, tree='isttokmarte', shot=51618):
        self.conn.openTree(tree, shot)

    def getData(self, node):
        return self.conn.get(node).data()

    def getTime(self, node):
        # print('dim_of({})'.format(node))
        return self.conn.get('dim_of({})'.format(node)).data()
