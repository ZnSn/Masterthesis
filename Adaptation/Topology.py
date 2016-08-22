
from mininet.node import Host, RemoteController, UserSwitch, OVSSwitch,\
    OVSController, Controller
from mininet.topo import Topo
from mininet.util import quietRun
from mininet.log import error
from Defines import *

def add_topo(net):
        "Create custom topo."

        # Add hosts and switches
        leftHost_v1 = net.addHost( 'h1' , ip='10.0.0.1')
        rightHost_v1 = net.addHost( 'h2', ip='10.0.0.2')

        left_marker = net.addSwitch ( 's20', dpid='20', protocols='OpenFlow13')
        right_marker = net.addSwitch ( 's21', dpid='21', protocols='OpenFlow13')

        switches = [None] # Add None object on position 0 to be consistent with switch numbering
        for i in range(1,11):
            switches.append(net.addSwitch( 's%s' % i, dpid=str(i), protocols='OpenFlow13'))

        # Add links from nodes to marker
        net.addLink( node1=leftHost_v1, node2=left_marker, port1=1, port2=1 )
        net.addLink( node1=right_marker, node2=rightHost_v1, port1=2, port2=2 )
        
        # Add edge links of topology 
        net.addLink( node1=left_marker, node2=switches[1], port1=2, port2=3)
        net.addLink( node1=switches[6], node2=right_marker, port1=1, port2=1 )    
        
        # Add links of topology
        net.addLink( node1=switches[1], node2=switches[2], port1=1, port2=3)
        
        net.addLink( node1=switches[2], node2=switches[3], port1=1, port2=3)
        net.addLink( node1=switches[2], node2=switches[7], port1=2, port2=3)
        
        net.addLink( node1=switches[3], node2=switches[8], port1=1, port2=3) 
        net.addLink( node1=switches[3], node2=switches[4], port1=2, port2=3) 
        
        net.addLink( node1=switches[4], node2=switches[5], port1=1, port2=4) 
        net.addLink( node1=switches[5], node2=switches[6], port1=1, port2=3)
        
        net.addLink( node1=switches[7], node2=switches[5], port1=1, port2=5)
        
        net.addLink( node1=switches[8], node2=switches[9], port1=1, port2=3)
        net.addLink( node1=switches[9], node2=switches[10], port1=1, port2=3)
        net.addLink( node1=switches[10], node2=switches[5], port1=1, port2=3)
        
        
if __name__ == '__main__':
    import sys
    from functools import partial

    from mininet.net import Mininet
    from mininet.cli import CLI
    from mininet.log import setLogLevel

    setLogLevel( 'info' )

    net = Mininet(topo = None, autoSetMacs=True, autoStaticArp=True, switch=OVSSwitch, build=False)#, controller=RemoteController)#, autoSetMacs=True )#, controller=RemoteController)
    
    
    print ('*** Adding Controllers\n')
    #c1 = net.addController(name='c1', controller=Controller, port=6634)
    c1 = net.addController(name='c0', controller=RemoteController, port=6633)
        
    print ('*** Adding Topology\n')
    add_topo(net)
    
    print ( '*** Starting network\n')
    net.start()
    
    CLI( net )
    net.stop()