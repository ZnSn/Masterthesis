'''
Created on Mar 2, 2016

@author: stephan
'''


# To be removed in final library

SWITCH_LEFT_ID = 0x20
SWITCH_RIGHT_ID = 0x21

VLAN_ID=0xa

H1_SRC = '00:00:00:00:00:01'
H1_IPv4 = '10.0.0.1'

H2_SRC = '00:00:00:00:00:02'
H2_IPv4 = '10.0.0.2'


PCP_INITIAL = 4
DSCP_INITIAL = PCP_INITIAL

STEP = 1
INCREASE = STEP
DECREASE = -STEP
ADAPTION_TYPE = INCREASE

NUMBER_OPERATIONS = 2


# Library defines

#===============================================================================
# Available Operators For The Routing Decision
#===============================================================================
class OPERATOR:
    SMALLER = 'smaller'
    GREATER = 'greater'
    EQUAL = 'equal'

#===============================================================================
# Class Holding All Available Adaption Definitions
#===============================================================================
class ADAPTION:
    PCP_MIN = 0
    PCP_MAX = 7
    INCREASE_STEP = 1
    DECREASE_STEP = -1
    NO_STEP = 0

#===============================================================================
# Class For Naming Certain Fields (e.g. in dictionaries)
#===============================================================================
class NAMING:
    VLAN = 'VLAN'
    DSCP = 'DSCP'
    TAGS = 'Tags'
    COMPARISON = 'Comparison'
    OPERATOR = 'Operator'
    VALUE = 'Value'
    SWITCH = 'Switch'

#===============================================================================
# Available Operations To Adapt PCP /  DSCP
#===============================================================================
class OPERATION:
    INCREASE = 'increase'
    DECREASE = 'decrease'
    IDENTITY = 'identity'

#===============================================================================
# Levels Of Priority -> Level 2 (PCP) and Level 3 (DSCP)
#===============================================================================
class PRIORITY_LEVEL: 
    PCP = 0
    DSCP = 1

#===============================================================================
# Table numbers 
#===============================================================================
class TABLE_NUMBER:
    TAG_OPERATION_TABLE = 0
    
    INCREASE_TABLE = 1
    DECREASE_TABLE = 2

    TAG_COMPARISON_TABLE = NUMBER_OPERATIONS + 1
    
    THRESHOLD_START = TAG_COMPARISON_TABLE + 1   
    # Ranges 21-50 reserved for comparison tables
    
    ROUTING_TABLE_HIGH = 255
    ROUTING_TABLE_LOW = 254
    
   

#===============================================================================
# Return Codes
#===============================================================================
class RETURN_CODE:
    IL_ERROR =      -1  # Error occured, see log file for despription (TODO)
    IL_OK =         0   # Everything is OK
    IL_NO_UPDATE =  1   # No update was necessary, nothing happened


#===============================================================================
# Match Fields
#===============================================================================
class MATCH_FIELD:
    
#################################################################################################
#   Access Name           Match name        Type of match       Description                     #
#################################################################################################
    
    IN_PORT         =     'in_port'         #Integer 32bit      Switch input port
    IN_PHY_PORT     =     'in_phy_port'     #Integer 32bit      Switch physical input port
    METADATA        =     'metadata'        #Integer 64bit      Metadata passed between tables
    ETH_DST         =     'eth_dst'         #MAC address        Ethernet destination address
    ETH_SRC         =     'eth_src'         #MAC address        Ethernet source address
    ETH_TYPE        =     'eth_type'        #Integer 16bit      Ethernet frame type
    VLAN_VID        =     'vlan_vid'        #Integer 16bit      VLAN id
    VLAN_PCP        =     'vlan_pcp'        #Integer 8bit       VLAN priority
    IP_DSCP         =     'ip_dscp'         #Integer 8bit       IP DSCP (6 bits in ToS field)
    IP_ECN          =     'ip_ecn'          #Integer 8bit       IP ECN (2 bits in ToS field)
    IP_PROTO        =     'ip_proto'        #Integer 8bit       IP protocol
    IPV4_SRC        =     'ipv4_src'        #IPv4 address       IPv4 source address
    IPV4_DST        =     'ipv4_dst'        #IPv4 address       IPv4 destination address
    TCP_SRC         =     'tcp_src'         #Integer 16bit      TCP source port
    TCP_DST         =     'tcp_dst'         #Integer 16bit      TCP destination port
    UDP_SRC         =     'udp_src'         #Integer 16bit      UDP source port
    UDP_DST         =     'udp_dst'         #Integer 16bit      UDP destination port
    SCTP_SRC        =     'sctp_src'        #Integer 16bit      SCTP source port
    SCTP_DST        =     'sctp_dst'        #Integer 16bit      SCTP destination port
    ICMPV4_TYPE     =     'icmpv4_type'     #Integer 8bit       ICMP type
    ICMPV4_CODE     =     'icmpv4_code'     #Integer 8bit       ICMP code
    ARP_OP          =     'arp_op'          #Integer 16bit      ARP opcode
    ARP_SPA         =     'arp_spa'         #IPv4 address       ARP source IPv4 address
    ARP_TPA         =     'arp_tpa'         #IPv4 address       ARP target IPv4 address
    ARP_SHA         =     'arp_sha'         #MAC address        ARP source hardware address
    ARP_THA         =     'arp_tha'         #MAC address        ARP target hardware address
    IPV6_SRC        =     'ipv6_src'        #IPv6 address       IPv6 source address
    IPV6_DST        =     'ipv6_dst'        #IPv6 address       IPv6 destination address
    IPV6_FLABEL     =     'ipv6_flabel'     #Integer 32bit      IPv6 Flow Label
    ICMPV6_TYPE     =     'icmpv6_type'     #Integer 8bit       ICMPv6 type
    ICMPV6_CODE     =     'icmpv6_code'     #Integer 8bit       ICMPv6 code
    IPV6_ND_TARGET  =     'ipv6_nd_target'  #IPv6 address       Target address for ND
    IPV6_ND_SLL     =     'ipv6_nd_sll'     #MAC address        Source link-layer for ND
    IPV6_ND_TLL     =     'ipv6_nd_tll'     #MAC address        Target link-layer for ND
    MPLS_LABEL      =     'mpls_label'      #Integer 32bit      MPLS label
    MPLS_TC         =     'mpls_tc'         #Integer 8bit       MPLS TC
    MPLS_BOS        =     'mpls_bos'        #Integer 8bit       MPLS BoS bit
    PBB_ISID        =     'pbb_isid'        #Integer 24bit      PBB I-SID
    TUNNEL_ID       =     'tunnel_id'       #Integer 64bit      Logical Port Metadata
    IPV6_EXTHDR     =     'ipv6_exthdr'     #Integer 16bit      IPv6 Extension Header pseudo-field
    PBB_UCA         =     'pbb_uca'         #Integer 8bit       PBB UCA header field (EXT-256 Old version of ONF Extension)
    TCP_FLAGS       =     'tcp_flags'       #Integer 16bit      TCP flags (EXT-109 ONF Extension)
    ACTSET_OUTPUT   =     'actset_output'   #Integer 32bit      Output port from action set metadata (EXT-233 ONF Extension)
    
    # HARDCODE OPTIONS
    @classmethod
    def get_options(cls):      
        return [cls.IN_PORT, cls.IN_PHY_PORT, cls.METADATA, cls.ETH_DST, cls.ETH_SRC, \
                cls.ETH_TYPE, cls.VLAN_VID, cls.VLAN_PCP, cls.IP_DSCP, cls.IP_ECN, \
                cls.IP_PROTO, cls.IPV4_SRC, cls.IPV4_DST, cls.TCP_SRC, cls.TCP_DST, \
                cls.UDP_SRC, cls.UDP_DST, cls.SCTP_SRC, cls.SCTP_DST, cls.ICMPV4_TYPE, \
                cls.ICMPV4_CODE, cls.ARP_OP, cls.ARP_SPA, cls.ARP_TPA, cls.ARP_SHA, \
                cls.ARP_THA, cls.IPV6_SRC, cls.IPV6_DST, cls.IPV6_FLABEL, cls.ICMPV6_TYPE, \
                cls.ICMPV6_CODE, cls.IPV6_ND_TARGET, cls.IPV6_ND_SLL, cls.IPV6_ND_TLL, \
                cls.MPLS_LABEL, cls.MPLS_TC, cls.MPLS_BOS, cls.PBB_ISID, cls.TUNNEL_ID, \
                cls.IPV6_EXTHDR, cls.PBB_UCA, cls.TCP_FLAGS, cls.ACTSET_OUTPUT]
  
class OFP_ACTIONS:
    
    pass


class OFP_INSTRUCTIONS:
    
    pass


    
