################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Backup/common.c \
../Backup/dpdksender.c \
../Backup/fifoqueue.c 

OBJS += \
./Backup/common.o \
./Backup/dpdksender.o \
./Backup/fifoqueue.o 

C_DEPS += \
./Backup/common.d \
./Backup/dpdksender.d \
./Backup/fifoqueue.d 


# Each subdirectory must supply rules for building sources it contributes
Backup/%.o: ../Backup/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


