################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/consola/consola.c \
../src/consola/funcionesConsola.c 

OBJS += \
./src/consola/consola.o \
./src/consola/funcionesConsola.o 

C_DEPS += \
./src/consola/consola.d \
./src/consola/funcionesConsola.d 


# Each subdirectory must supply rules for building sources it contributes
src/consola/%.o: ../src/consola/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


