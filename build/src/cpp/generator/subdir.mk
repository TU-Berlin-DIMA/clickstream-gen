################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/cpp/generator/GeneratorSubsystem.cpp 

OBJS += \
./src/cpp/generator/GeneratorSubsystem.o 

CPP_DEPS += \
./src/cpp/generator/GeneratorSubsystem.d 


# Each subdirectory must supply rules for building sources it contributes
src/cpp/generator/%.o: ../src/cpp/generator/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"$(POCO_INCLUDE_PATH)" -I"../vendor/myriad/src/cpp" -I"../src/cpp" -O3 -g -Wall -c -fmessage-length=0 -falign-functions -DNDEBUG -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


