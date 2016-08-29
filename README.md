# Masterthesis: In-network Packet Priority Adaptation for Networked Control Systems
##### Stephan Zinkler - 2016
#
#
### Introduction
This repository contains all relavant implementations developed as a part of the masterthesis for "In-network Packet Priority Adaptation for Networked Control Systems" at the Distributed Systems Department, Insititue for Parallel and Distributed Systems (IPVS) at the university of Stuttgart.

### Abstract
Sharing the network between Networked Control System (NCS) having strict demands
with respect to latency and jitter and applications only requiring best-effort service leads
to multiple problems. An important task to consider is how to prioritize individual types
of traffic in such a way that the necessary guarantees for an NCS to be stable can still
be given. While there are ways to prioritize the more important control traffic of an
NCS over best-effort traffic sharing the same network, a more sophisticated approach
has to be found in order to handle multiple NCS sharing the highest priority. In this
thesis, in-network priority scheduling applications with a global view on the network
are developed in order to schedule and prioritize individual NCS such that their stability
can be guaranteed while sharing the network between multiple NCS.

This thesis deals with in-network packet priority scheduling for Networked Control
Systems. Using Data Plane Development Kit (DPDK) to achieve a Network Function
Virtualization (NFV) based approach, a priority scheduling application is implemented in
a middlebox to handle continuous priorities. This application could be instantiated and
migrated within the network while simultaneously using Software Defined Networking
(SDN) to route the traffic to the respective nodes. Additionally, this approach is extended
using SDN and OpenFlow to adapt priorities in-network. Using the eight internal per-
port queues of a switch, discrete priorities are used to schedule, and additionally adapt,
the priorities on the switch. This approach could give the opportunity for priority-based
routing by using the SDN-controller for routing decisions and configuring the switches.

The evaluation of this thesis is done by simulating NCSs and emulating the network
containing the middlebox. For this, a simulation of an inverted pendulum is implemented
for which the use of DPDK is compared to standard sockets. It can be shown that DPDK
is able to perform better due to less delay and jitter. The scheduling application is
evaluated by comparing it to a round-robin scheduling approach. The result suggests
that the application is able to keep multiple NCS more stable than it’s round-robin
counterpart. Furthermore, it is able to stabilize a more unstable system faster and more
effectively. While the maximum sampling time for a system with a pendulum having an
initial angle of 35° was found to be 50ms for the round-robin scheme, the middlebox is
able to keep the system stable until 120ms. The application using OpenFlow is evaluated
with respect to the time it takes to configure the switch as well as the overhead imposed
by the configuration compared to the number of NCS within the network

### Folder Structure
- **Adaptation**: Contains the In-network Packet Priority Adaptation OpenFlow library
- **NCS_Simulation**: Contains both the DPDK sender/receiver application and the Networked Control System simulation library
- **Priority_Scheduling**: Contains the priority scheduling dpdk application as well as the round robin counterpart.

### Required Packages
- Python Packages
- Ryu
- DPDK
- Armadillo (C++)
