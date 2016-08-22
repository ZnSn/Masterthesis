import xml.etree.ElementTree as ET
from Defines import *

def load_config(filepath):

    settings = {}

    with open(filepath) as f:
        root = ET.parse(f).getroot()
        for switch_node in root.findall('Switch'):
            switch_id = int(switch_node.attrib['id'])
            settings[switch_id] = {NAMING.TAGS : {}, NAMING.COMPARISON : {}}

            for tag in switch_node.findall(NAMING.VLAN):
                tag_id = int(tag.attrib['id'])
                operation = tag.attrib['operation']
                settings[switch_id][NAMING.TAGS][tag_id] = operation
                
            threshold_node = switch_node.find(NAMING.COMPARISON)
            if threshold_node is None:
                # All traffic is high -> one route!
                op = 'smaller'
                value = 8
            else:
                op = threshold_node.attrib['operator']
                value = int(threshold_node.attrib['value'])

            settings[switch_id][NAMING.COMPARISON]['Operator'] = op
            settings[switch_id][NAMING.COMPARISON]['Value'] = value

    return settings

def operation_to_table(operation_string):

    op = operation_string.lower()

    if op == OPERATION.INCREASE:
        return TABLE_NUMBER.INCREASE_TABLE

    elif op == OPERATION.DECREASE:
        return TABLE_NUMBER.DECREASE_TABLE

    elif op == OPERATION.IDENTITY:
        return TABLE_NUMBER.TAG_COMPARISON_TABLE