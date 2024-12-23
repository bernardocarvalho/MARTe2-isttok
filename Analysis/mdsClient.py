""""
"""
import mdsthin


class mdsClient():
    """
    Models
    """

    def __init__(self, host='epics.ipfn.tecnico.ulisboa.pt'):
        """
        Initializes the MDSPlus thin Connection
        """
        self.conn = mdsthin.Connection(f'ssh://{host}')

    def openTree(self, tree='isttokmarte', shot=51608):
        self.conn.openTree(tree, shot)

    def getData(self, node):
        return self.conn.get(node).data()

    def getTime(self, node):
        # print('dim_of({})'.format(node))
        return self.conn.get('dim_of({})'.format(node)).data()
