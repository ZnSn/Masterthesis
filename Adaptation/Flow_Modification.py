'''
Created on Mar 14, 2016

@author: stephan
'''
import Tools
from Defines import PRIORITY_LEVEL, RETURN_CODE, TABLE_NUMBER, ADAPTION, NAMING, OPERATION,\
    OPERATOR, MATCH_FIELD
from ryu.ofproto import ofproto_v1_3
from ryu.ofproto.ofproto_v1_3 import OFPG_ANY

class Flow_Modification():
    
    def __init__(self):
                
        self.func_tag_operation = self.install_table_tag_operation_pcp
        self.func_priority_adaption = self.install_table_priority_adaption_pcp
        self.func_tag_comparison = self.install_table_tag_comparison_pcp
        self.func_priority_comparison = self.install_table_priority_comparison_pcp
        self.func_routing = self.install_table_routing_pcp
        
        self.level = PRIORITY_LEVEL.PCP
        self.previous_level = self.level
        
    #===========================================================================
    # Function Pointer Settings For Dynamic Level Switching
    #===========================================================================
        
    def use_pcp(self):
        
        self.previous_level = self.level
        
        self.func_tag_operation = self.install_table_tag_operation_pcp
        self.func_priority_adaption = self.install_table_priority_adaption_pcp
        self.func_tag_comparison = self.install_table_tag_comparison_pcp
        self.func_priority_comparison = self.install_table_priority_comparison_pcp
        self.func_routing = self.install_table_routing_pcp
        
        self.level = PRIORITY_LEVEL.PCP
        
    def use_dscp(self):
        
        self.previous_level = self.level
        
        self.func_tag_operation = self.install_table_tag_operation_dscp
        self.func_priority_adaption = self.install_table_priority_adaption_dscp
        self.func_tag_comparison = self.install_table_tag_comparison_dscp
        self.func_priority_comparison = self.install_table_priority_comparison_dscp
        self.func_routing = self.install_table_routing_dscp
        
        self.level = PRIORITY_LEVEL.DSCP
        
    def get_level(self):
        return self.level

    #===========================================================================
    # Entry Function For Pipeline Creation / Modification 
    #===========================================================================

    def install_pipeline(self, datapath, switch):
                    
        self.func_tag_operation(datapath, switch)
        self.func_priority_adaption(datapath)
        
        self.func_tag_comparison(datapath, switch)
        self.func_priority_comparison(datapath, switch)
        
        self.func_routing(datapath, switch)
        
    def delete_all_tables(self):
        pass
        
    def reconfigure_pipeline(self, datapath, switch_id):
        '''
        Called whenever the topology switches from Level 2 to Level 3 and vice versa.
        Deletes deprecated flow tables and replaces them with the respective correct one.
        '''

        return RETURN_CODE.IL_OK
    
    # TODO: Remove settings from here -> respective part as function parameter!!!
        
        
    #===========================================================================
    # PCP Flow Tables Installation
    #===========================================================================

    def install_table_tag_operation_pcp(self, datapath, switch):
        '''
        Installs table 0 on the switch deciding on which flow values what operation should be the next pipeline hop
        '''

        print 'INSTALLING TABLE TAG OPERATION ON SWITCH %s' % switch.get_id()
        ofp_parser = datapath.ofproto_parser
        
        flow_ids = switch.get_flow_ids()
                        
        for flow_id in flow_ids:
                        
            next_table = switch.get_operation_table_number(flow_id)        
            
            flow_matches = switch.get_flow(flow_id)
                            
            #  Special handling of known problems
            flow_matches_adapted = self.__handle_matches(flow_matches)

            match = ofp_parser.OFPMatch(**flow_matches_adapted)
            instructions = [ofp_parser.OFPInstructionGotoTable(next_table)] 
            self.add_flow(datapath=datapath, priority=1, match=match, actions=[], table=TABLE_NUMBER.TAG_OPERATION_TABLE, instructions=instructions)


    def install_table_priority_adaption_pcp(self, datapath):
        '''
        Installs the available operations on the switch
        '''
        self.install_operation_increase(datapath)
        self.install_operation_decrease(datapath)
        
    def install_table_tag_comparison_pcp(self, datapath, switch):
        
        print 'INSTALLING TABLE TAG COMPARISON ON SWITCH %s' % switch.get_id()
        
        ofp_parser = datapath.ofproto_parser
        flow_ids = switch.get_flow_ids()
        
        for flow_id in flow_ids:
            
            table_num = switch.get_threshold_table_number(flow_id)
            flow_matches = switch.get_flow(flow_id)
                
            #  Special handling of known problems
            flow_matches_adapted = self.__handle_matches(flow_matches)

            match = ofp_parser.OFPMatch(**flow_matches_adapted)
            instructions = [ofp_parser.OFPInstructionGotoTable(table_num)] 
            self.add_flow(datapath=datapath, priority=1, match=match, actions=[], table=TABLE_NUMBER.TAG_COMPARISON_TABLE, instructions=instructions)
            

    def install_table_priority_comparison_pcp(self, datapath, switch):
        '''
        Installs the decision table deciding on when the high priority and when the low priority path should be used
        '''
        
        
        print 'INSTALLING TABLE PRIORITY COMPARISON ON SWITCH %s' % switch.get_id()
        
        ofp_parser = datapath.ofproto_parser
        flow_ids = switch.get_flow_ids()
        
        for flow_id in flow_ids:
            
            table_num = switch.get_threshold_table_number(flow_id)
            threshold = switch.get_threshold_value(flow_id)

            low = [x for x in range(ADAPTION.PCP_MIN, threshold+1)]
            high = [x for x in range(threshold+1, ADAPTION.PCP_MAX+1)]

            for pcp in high:
                match = ofp_parser.OFPMatch(vlan_vid=(ofproto_v1_3.OFPVID_PRESENT,ofproto_v1_3.OFPVID_PRESENT), vlan_pcp=pcp)
                actions = []
                instructions = [ofp_parser.OFPInstructionGotoTable(TABLE_NUMBER.ROUTING_TABLE_HIGH)] 
                self.add_flow(datapath=datapath, priority=1, match=match, actions=actions, table=table_num, instructions=instructions)
            
            for pcp in low:
                match = ofp_parser.OFPMatch(vlan_vid=(ofproto_v1_3.OFPVID_PRESENT,ofproto_v1_3.OFPVID_PRESENT), vlan_pcp=pcp)
                actions = []
                instructions = [ofp_parser.OFPInstructionGotoTable(TABLE_NUMBER.ROUTING_TABLE_LOW)] 
                self.add_flow(datapath=datapath, priority=1, match=match, actions=actions, table=table_num, instructions=instructions)
        

    def install_table_routing_pcp(self, datapath, switch):
        '''
        Installs the routing table on the switch
        '''

        # TODO: CORRECT ROUTING DEPENDING ON PATH GIVEN BY NORTHBOUND INTERFACE
        
        print 'INSTALLING TABLE ROUTING ON SWITCH %s' % switch.get_id()
        
        ofp_parser = datapath.ofproto_parser
        flow_ids = switch.get_flow_ids()
        
        for flow_id in flow_ids:
            
            flow_matches = switch.get_flow(flow_id)
            
            out_port_high = switch.get_high_priority_out_port(flow_id)
            out_port_low = switch.get_low_priority_out_port(flow_id)
            
            #  Special handling of known problems
            flow_matches_adapted = self.__handle_matches(flow_matches)
            
            if out_port_high is None and out_port_low is None:
                print 'No Output Ports specified -> No routing applied on switch %s' % switch.get_id()
                
                sid = switch.get_id()
                if sid == 6:
                    outport = 1
                    
                    
                if sid == 1:
                    outport = 3
                
                # Seperate handling of edge routers -> To be removed!!
                match = ofp_parser.OFPMatch(**flow_matches_adapted)
                actions = [ofp_parser.OFPActionOutput(int(outport))]
                self.add_flow(datapath=datapath, priority=10, match=match, actions=actions, table=TABLE_NUMBER.ROUTING_TABLE_HIGH)
                
                match = ofp_parser.OFPMatch(**flow_matches_adapted)
                actions = [ofp_parser.OFPActionOutput(int(outport))]
                self.add_flow(datapath=datapath, priority=10, match=match, actions=actions, table=TABLE_NUMBER.ROUTING_TABLE_LOW)
                
                continue 
            
            
            '''
            # Route only if ports are given!
            if out_port_high is not None:
                                                   
                match = ofp_parser.OFPMatch(**flow_matches_adapted)
                actions = [ofp_parser.OFPActionOutput(out_port_high)]
                self.add_flow(datapath=datapath, priority=10, match=match, actions=actions, table=TABLE_NUMBER.ROUTING_TABLE_HIGH)
                
            else:
                print 'No Output Ports for high prio specified -> No routing applied on switch %s' % switch.get_id()                
            
            if out_port_low is not None:
    
                match = ofp_parser.OFPMatch(**flow_matches_adapted)
                actions = [ofp_parser.OFPActionOutput(out_port_low)]
                self.add_flow(datapath=datapath, priority=10, match=match, actions=actions, table=TABLE_NUMBER.ROUTING_TABLE_LOW)
            
            else:
                print 'No Output Ports for low prio specified -> No routing applied on switch %s' % switch.get_id()   
                
            '''
            
            if out_port_high is None: out_port_high = out_port_low
            if out_port_low is None: out_port_low = out_port_high
            
            match = ofp_parser.OFPMatch(**flow_matches_adapted)
            actions = [ofp_parser.OFPActionOutput(int(out_port_high))]
            self.add_flow(datapath=datapath, priority=10, match=match, actions=actions, table=TABLE_NUMBER.ROUTING_TABLE_HIGH)
            
            match = ofp_parser.OFPMatch(**flow_matches_adapted)
            actions = [ofp_parser.OFPActionOutput(int(out_port_low))]
            self.add_flow(datapath=datapath, priority=10, match=match, actions=actions, table=TABLE_NUMBER.ROUTING_TABLE_LOW)

    def install_operation_increase(self, datapath):
        '''
        Install the table for the increase of the pcp by 1 on the switch
        ''' 

        for i in range(ADAPTION.PCP_MIN,ADAPTION.PCP_MAX):
            match, actions, instructions = self.create_match_actions_instructions_vlan_goto(datapath, i, ADAPTION.INCREASE_STEP, TABLE_NUMBER.TAG_COMPARISON_TABLE)
            self.add_flow(datapath=datapath, priority=0, match=match, actions=actions, table=TABLE_NUMBER.INCREASE_TABLE, instructions=instructions)
        
        match, actions, instructions = self.create_match_actions_instructions_vlan_goto(datapath, ADAPTION.PCP_MAX, ADAPTION.NO_STEP, TABLE_NUMBER.TAG_COMPARISON_TABLE)
        self.add_flow(datapath=datapath, priority=0, match=match, actions=actions, table=TABLE_NUMBER.INCREASE_TABLE, instructions=instructions)

    def install_operation_decrease(self, datapath):
        '''
        Install the table for the decrease of the pcp by 1 on the switch
        ''' 
                    
        for i in range(ADAPTION.PCP_MIN+1,ADAPTION.PCP_MAX+1):
            match, actions, instructions = self.create_match_actions_instructions_vlan_goto(datapath, i, ADAPTION.DECREASE_STEP, TABLE_NUMBER.TAG_COMPARISON_TABLE)
            self.add_flow(datapath=datapath, priority=0, match=match, actions=actions, table=TABLE_NUMBER.DECREASE_TABLE, instructions=instructions)
        
        match, actions, instructions = self.create_match_actions_instructions_vlan_goto(datapath, ADAPTION.PCP_MIN, ADAPTION.NO_STEP, TABLE_NUMBER.TAG_COMPARISON_TABLE)
        self.add_flow(datapath=datapath, priority=0, match=match, actions=actions, table=TABLE_NUMBER.DECREASE_TABLE, instructions=instructions)

    # TODO: ADD MORE OPERATIONS
    
    
    #===========================================================================
    # DSCP Flow Tables Installation -> Not used / needed in this thesis
    #===========================================================================
    
    def install_table_tag_operation_dscp(self, datapath, switch):
        pass
    
    def install_table_priority_adaption_dscp(self, datapath):
        pass
    
    def install_table_tag_comparison_dscp(self, datapath, switch):
        pass
        
    def install_table_priority_comparison_dscp(self, datapath, switch):
        pass
    
    def install_table_routing_dscp(self, datapath):
        pass
    
    #===========================================================================
    # Flow Creation Methods
    #===========================================================================

    def create_match_actions_instructions_vlan_goto(self, datapath, priority, step, goto):
        
        ofproto = datapath.ofproto
        ofp_parser = datapath.ofproto_parser
    
        f = ofp_parser.OFPMatchField.make(ofproto.OXM_OF_VLAN_PCP, priority+step)
        
        match = ofp_parser.OFPMatch(vlan_vid=(ofproto_v1_3.OFPVID_PRESENT, ofproto_v1_3.OFPVID_PRESENT), vlan_pcp=priority)
        actions = [ofp_parser.OFPActionSetField(f)]
        instructions = [ofp_parser.OFPInstructionActions(ofproto.OFPIT_APPLY_ACTIONS, actions), ofp_parser.OFPInstructionGotoTable(goto)] 

        return match, actions, instructions


    # Add a flow table entry to the switch definied by <datapath> matching the entries described in <match> and apply <actions>
    def add_flow(self, datapath, priority, match, actions, table=0, instructions=None, buffer_id=None):

        ofproto = datapath.ofproto
        parser = datapath.ofproto_parser

        if instructions is None:
            inst = [parser.OFPInstructionActions(ofproto.OFPIT_APPLY_ACTIONS, actions)]
        else: inst = instructions

        if buffer_id:
            mod = parser.OFPFlowMod(datapath=datapath, buffer_id=buffer_id,
                            priority=priority, match=match,
                            instructions=inst, table=table)
        else:
            mod = parser.OFPFlowMod(datapath=datapath, priority=priority,
                            match=match, instructions=inst, table_id=table)
        datapath.send_msg(mod)
        
    #===========================================================================
    # Helper Methods
    #===========================================================================
    
    def __handle_matches(self, match_dict):
        
        # VLAN VID has to have <ofproto_v1_3.OFPVID_PRESENT> in the field -> greater then 0x1000!
        if MATCH_FIELD.VLAN_VID in match_dict:
            value = match_dict[MATCH_FIELD.VLAN_VID]
            if value < ofproto_v1_3.OFPVID_PRESENT:
                match_dict[MATCH_FIELD.VLAN_VID] = value | ofproto_v1_3.OFPVID_PRESENT
                
        # If PCP is used, VLAN has to be present -> Set vlan_vid to "vlan present"
        if MATCH_FIELD.VLAN_PCP in match_dict and MATCH_FIELD.VLAN_VID not in match_dict:
            match_dict[MATCH_FIELD.VLAN_VID] = (ofproto_v1_3.OFPVID_PRESENT,ofproto_v1_3.OFPVID_PRESENT)
        
        
        return match_dict
