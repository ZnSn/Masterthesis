'''
Created on Mar 23, 2016

@author: stephan
'''
from Defines import *
import Tools

# Hardcoded port dict:
# p_dict[switch_id][next_switch] = port
p_dict = {}

p_dict[1] = {}
p_dict[1][2] = 1

p_dict[2] = {}
p_dict[2][1] = 3
p_dict[2][3] = 1
p_dict[2][7] = 2

p_dict[3] = {}
p_dict[3][2] = 3
p_dict[3][8] = 1
p_dict[3][4] = 2

p_dict[4] = {}
p_dict[4][3] = 3
p_dict[4][5] = 1

p_dict[5] = {}
p_dict[5][10] = 3
p_dict[5][4] = 4
p_dict[5][7] = 5
p_dict[5][6] = 1

p_dict[6] = {}
p_dict[6][5] = 3

p_dict[7] = {}
p_dict[7][2] = 3
p_dict[7][5] = 1

p_dict[8] = {}
p_dict[8][3] = 3
p_dict[8][9] = 1

p_dict[9] = {}
p_dict[9][10] = 1
p_dict[9][8] = 3

p_dict[10] = {}
p_dict[10][9] = 3
p_dict[10][5] = 1

class Topology:
    
    def __init__(self):
        
        # Dictionary of {id : switch obj} for fast access
        self.switches = {} 
        self.flow_dict = {}
        
    #===========================================================================
    # Private Creation and Getter Methods
    #===========================================================================
        
    def __create_switch(self, switch_id):
        s = Switch(switch_id=switch_id)
        self.switches[switch_id] = s
        return s
        
    def __get_flow(self, flow_id):
        if flow_id in self.flow_dict:
            return self.flow_dict[flow_id]
        else: return None
        
    def __create_get_switch(self, switch_id):
        # Create or get switch with id <switch_id>
        s = self.get_switch(switch_id)
        if s is None:
            switch = self.__create_switch(switch_id)
        else: switch = s
        
        return switch
    
    def __create_path_element(self, flow_id, switch_id):
        
        s = self.get_switch(switch_id)
            
        if s is None: # Create new switch and add initial operation and threshold
            switch = self.__create_switch(switch_id)
        else:
            switch = s
            
        # Add flow to switch
        switch.add_operation(flow_id, OPERATION.IDENTITY)
        switch.add_threshold(flow_id, 0)
        switch.add_flow(flow_id, self.__get_flow(flow_id))
        
        return switch
    
    #===========================================================================
    # Public Creation Functions
    #===========================================================================
       
    def create_flow(self, flow_id, **kwargs):
        
        self.flow_dict[flow_id] = {}
  
        for key, value in kwargs.items():
            if key not in MATCH_FIELD.get_options():
                print 'Create Flow: %s is not supported!' % key
                continue
            
            self.flow_dict[flow_id][key] = value
            
            
    def create_path_low(self, flow_id, switch_id_list):
        
        
        for switch_id, out_port in switch_id_list:
            
            switch = self.__create_path_element(flow_id, switch_id)
            
            if out_port is not None:
                
                # Routing tables            
                switch.add_port_low_priority(flow_id, out_port)
            
                                    
    def create_path_high(self, flow_id, switch_id_list):
        
        
        for switch_id, out_port in switch_id_list:
            
            switch = self.__create_path_element(flow_id, switch_id)
            
            if out_port is not None:
                
                # Routing tables            
                switch.add_port_high_priority(flow_id, out_port)
            
            
    def create_path(self, flow_id, switch_id_list):
                
        for switch_id, out_port in switch_id_list:
            
            switch = self.__create_path_element(flow_id, switch_id)
            
            if out_port is not None:
                
                switch.add_port_high_priority(flow_id, out_port)
                switch.add_port_low_priority(flow_id, out_port)


    #===========================================================================
    # Public Setter Functions
    #===========================================================================
    
    def set_operation(self, flow_id, switch_id, operation):
        
        switch = self.get_switch(switch_id)
        switch.set_operation(flow_id, operation)
        
    def set_threshold(self, flow_id, switch_id, threshold):
        
        switch = self.get_switch(switch_id)
        switch.set_threshold(flow_id, threshold)
            
    #===========================================================================
    # Getter Functions
    #===========================================================================
            
    def get_switch_ids(self):
        return self.switches.keys()
    
    def get_switch(self, switch_id):
        if switch_id in self.switches:
            return self.switches[switch_id]
        else: return None
      
class Switch:
    
    def __init__(self, switch_id):     
        self.id = switch_id
        self.flow_dict = {}                 # { flow_id : { match_field_key : match_field_value } }
        self.thresholds = {}            # { flow_id : value }
        self.operations = {}            # { flow_id : operation }
        self.threshold_to_table = {}    # { flow_id : table number }
        self.low_priority_routing = {}  # { flow_id : out_port } 
        self.high_priority_routing = {} # { flow_id : out_port }
        
    #===========================================================================
    # Methods for adding flow settings to the switch
    #===========================================================================
        
    def add_flow(self, flow_id, flow_dict):
        
        if flow_id in self.flow_dict: return
        if flow_dict is None: return
        
        self.flow_dict[flow_id] = dict(flow_dict)
                   
    def add_threshold(self, flow_id, value):
        
        if flow_id not in self.thresholds:
            self.threshold_to_table[flow_id] = TABLE_NUMBER.THRESHOLD_START + len(self.thresholds)
            
        self.thresholds[flow_id] = value
         
    def add_operation(self, flow_id, operation):
                        
        self.operations[flow_id] = operation
                
    def add_port_low_priority(self, flow_id, port):
    
        self.low_priority_routing[flow_id] = port
        
    def add_port_high_priority(self, flow_id, port):
    
        self.high_priority_routing[flow_id] = port
            
    #===========================================================================
    # Getter Functions
    #===========================================================================
    
    def get_flow_ids(self):
        return self.flow_dict.keys()
    
    def get_flow(self, flow_id):
        return self.flow_dict[flow_id]
    
    def get_id(self):
        return self.id
    
    def get_threshold_table_number(self, flow_id):
        return self.threshold_to_table[flow_id]
    
    def get_threshold_value(self, flow_id):
        return self.thresholds[flow_id]
    
    def get_operation_table_number(self, flow_id):
        
        op = self.operations[flow_id]
        
        return Tools.operation_to_table(op)
    
    def get_low_priority_out_port(self, flow_id):
        if flow_id in self.low_priority_routing:
            return self.low_priority_routing[flow_id]

    def get_high_priority_out_port(self, flow_id):
        if flow_id in self.high_priority_routing:
            return self.high_priority_routing[flow_id]
       
    #===========================================================================
    # Setter Functions
    #===========================================================================
    
    def set_operation(self, flow_id, operation):
        
        self.add_operation(flow_id, operation)
        
    def set_threshold(self, flow_id, threshold):
        
        self.add_threshold(flow_id, threshold)
        
        
