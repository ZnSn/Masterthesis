cmd_dpdk_scheduling.o = gcc -Wp,-MD,./.dpdk_scheduling.o.d.tmp -m64 -pthread  -march=native -DRTE_MACHINE_CPUFLAG_SSE -DRTE_MACHINE_CPUFLAG_SSE2 -DRTE_MACHINE_CPUFLAG_SSE3 -DRTE_MACHINE_CPUFLAG_SSSE3 -DRTE_MACHINE_CPUFLAG_SSE4_1 -DRTE_MACHINE_CPUFLAG_SSE4_2 -DRTE_MACHINE_CPUFLAG_PCLMULQDQ -DRTE_MACHINE_CPUFLAG_AVX -DRTE_MACHINE_CPUFLAG_RDRAND -DRTE_MACHINE_CPUFLAG_FSGSBASE -DRTE_MACHINE_CPUFLAG_F16C -DRTE_MACHINE_CPUFLAG_AVX2  -I/home/zinklesn/round_robin_eval/build/include -I/home/zinklesn/Programs/dpdk-16.04/x86_64-native-linuxapp-gcc/include -include /home/zinklesn/Programs/dpdk-16.04/x86_64-native-linuxapp-gcc/include/rte_config.h -O3  -I/home/zinklesn/round_robin_eval/../shared  -O3 -g -o dpdk_scheduling.o -c /home/zinklesn/round_robin_eval/dpdk_scheduling.c 
