### Building the Simulation

Run the Makefiles in both the DPDK and the NCS_Library folders

### Running the Simulation

## The simulation using DPDK can be run via the following command:
sudo nice --adjustment=-20 <NCS_Simulation_folder>/DPDK/sender_receiver/build/app/sender_receiver -c 0xe -n4

## The simulation using Socket can be run via the following command:
<Socket_folder>/socket_app <IP-Adresse> <Port>



