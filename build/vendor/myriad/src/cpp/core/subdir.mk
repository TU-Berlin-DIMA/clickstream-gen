################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../vendor/myriad/src/cpp/core/Frontend.cpp \
../vendor/myriad/src/cpp/core/exceptions.cpp 

OBJS += \
./vendor/myriad/src/cpp/core/Frontend.o \
./vendor/myriad/src/cpp/core/exceptions.o 

CPP_DEPS += \
./vendor/myriad/src/cpp/core/Frontend.d \
./vendor/myriad/src/cpp/core/exceptions.d 


# Each subdirectory must supply rules for building sources it contributes
vendor/myriad/src/cpp/core/%.o: ../vendor/myriad/src/cpp/core/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"$(POCO_INCLUDE_PATH)" -I"../vendor/myriad/src/cpp" -I"../src/cpp" -O3 -g -Wall -c -fmessage-length=0 -falign-functions -DNDEBUG -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


