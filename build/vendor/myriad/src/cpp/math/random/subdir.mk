################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../vendor/myriad/src/cpp/math/random/RNG.cpp \
../vendor/myriad/src/cpp/math/random/SurrogateKeyGenerator.cpp 

OBJS += \
./vendor/myriad/src/cpp/math/random/RNG.o \
./vendor/myriad/src/cpp/math/random/SurrogateKeyGenerator.o 

CPP_DEPS += \
./vendor/myriad/src/cpp/math/random/RNG.d \
./vendor/myriad/src/cpp/math/random/SurrogateKeyGenerator.d 


# Each subdirectory must supply rules for building sources it contributes
vendor/myriad/src/cpp/math/random/%.o: ../vendor/myriad/src/cpp/math/random/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"$(POCO_INCLUDE_PATH)" -I"../vendor/myriad/src/cpp" -I"../src/cpp" -O3 -g -Wall -c -fmessage-length=0 -falign-functions -DNDEBUG -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


