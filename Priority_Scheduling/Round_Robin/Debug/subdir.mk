################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../common.c \
../dpdk_scheduling.c \
../dpdkreceiver.c \
../dpdksender.c \
../fifoqueue.c 

OBJS += \
./common.o \
./dpdk_scheduling.o \
./dpdkreceiver.o \
./dpdksender.o \
./fifoqueue.o 

C_DEPS += \
./common.d \
./dpdk_scheduling.d \
./dpdkreceiver.d \
./dpdksender.d \
./fifoqueue.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


