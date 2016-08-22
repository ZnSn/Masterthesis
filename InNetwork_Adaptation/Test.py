'''
Created on Mar 16, 2016

@author: stephan
'''

from Interface import *
from Defines import *

def test():
        
    graph = Topology()
    
    
    for i in range(1,17):
    # Create a flow with id 1 matching vlan_id 10 and eth_src '00:00:00:00:00:01'
		hex_str = "%0.2X" % i
		eth = '00:00:00:00:00:'+ hex_str
		print eth
		graph.create_flow(flow_id=i, vlan_vid=i, eth_src=eth)

		graph.create_path(flow_id=i, switch_id_list=[(11882589780257140512,10),(6,None)])

		graph.set_operation(flow_id=i, switch_id=11882589780257140512, operation=OPERATION.INCREASE)
		graph.set_threshold(flow_id=i, switch_id=11882589780257140512, threshold=4)

    return graph
    
   
if __name__ == '__main__': 
    test()
