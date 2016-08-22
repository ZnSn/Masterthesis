'''
Created on Mar 13, 2016

@author: stephan
'''

import time

from ryu.base import app_manager
from ryu.controller.handler import set_ev_cls, MAIN_DISPATCHER,\
    CONFIG_DISPATCHER
from ryu.controller import ofp_event
from ryu.lib.packet.packet import Packet
from ryu.lib.packet.ethernet import ethernet
from ryu.topology import event
from ryu.lib.packet.icmp import icmp
from ryu.ofproto import ofproto_v1_3, ether
from ryu.lib.packet.arp import arp

from Flow_Modification import  Flow_Modification
from Defines import *
import Tools
import Test

class Controller( app_manager.RyuApp):

	OFP_VERSIONS = [ofproto_v1_3.OFP_VERSION]

	def __init__(self, config='/home/zinklesn/Thesis/Config.xml', *args, **kwargs):
		super(Controller, self).__init__(*args, **kwargs)

		#self.__settings = Tools.load_config(config) # Settings dictionary to save settings for each registered switch
		self.topology = None
		
		self.flow_mod = Flow_Modification()
		
		# REMoVE
		self.Set_Topology(Test.test())
		
		for id in self.topology.get_switch_ids():
		    
		    print id, self.topology.get_switch(id).operations
		    
		self.switch_counter = 0
		self.timing_dict = {}
		
		
		
	#===========================================================================
	# Event Handlers
	#===========================================================================

	# Packet-In event called whenever switch doesn't know what to do whith an incoming packet
	@set_ev_cls(ofp_event.EventOFPPacketIn, MAIN_DISPATCHER)
	def packet_in_handler(self, ev):

		print "Packet In Handler called!"


	# After session initialization, controller sends feature request and the switch answers with feature response (this!)
	# Add respective flow entries here    
	@set_ev_cls(ofp_event.EventOFPSwitchFeatures, CONFIG_DISPATCHER)
	def switch_feature_event(self, ev):

		msg = ev.msg
		datapath = msg.datapath
		switch_id = msg.datapath_id
		ofproto = datapath.ofproto
		ofp_parser = datapath.ofproto_parser
		
		switch_ids = self.topology.get_switch_ids()
		
		# Install custom pipeline if switch is defined in config
		if switch_id in switch_ids:
		                
		    start = time.time()
		    self.flow_mod.install_pipeline(datapath, self.topology.get_switch(switch_id))
		    self.send_barrier_request(datapath)
		    self.timing_dict[self.switch_counter] = start
		    self.switch_counter += 1
		    
		    
	def send_barrier_request(self, datapath):
		ofp_parser = datapath.ofproto_parser
		
		req = ofp_parser.OFPBarrierRequest(datapath)
		req.set_xid(self.switch_counter)
		datapath.send_msg(req)
		
		
	@set_ev_cls(ofp_event.EventOFPBarrierReply, MAIN_DISPATCHER)
	def barrier_reply_handler(self, ev):
		
		end_time = time.time()
		
		switch_xid = ev.msg.xid
		intervall = end_time - self.timing_dict[switch_xid]
		
		self.logger.debug('---------------------------')
		self.logger.debug('OFPBarrierReply received')
		self.logger.debug(intervall)
		self.logger.debug('---------------------------')

















