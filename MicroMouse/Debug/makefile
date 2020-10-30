################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include startup/subdir.mk
-include WallFollowing/subdir.mk
-include Src/subdir.mk
-include Menu/subdir.mk
-include Maze/subdir.mk
-include MPU6050/subdir.mk
-include Libraries/subdir.mk
-include Language/subdir.mk
-include Fonts/subdir.mk
-include Drivers/STM32L4xx_HAL_Driver/Src/subdir.mk
-include DogModeTEST/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
MicroMouse.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
MicroMouse.list \

OBJCOPY_BIN += \
MicroMouse.bin \


# All Target
all:
	+@make --no-print-directory main-build && make --no-print-directory post-build

# Main-build Target
main-build: MicroMouse.elf secondary-outputs

# Tool invocations
MicroMouse.elf: $(OBJS) $(USER_OBJS) ../STM32L412R8_FLASH.ld
	arm-none-eabi-gcc -o "MicroMouse.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"../STM32L412R8_FLASH.ld" --specs=nosys.specs -Wl,-Map="MicroMouse.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

MicroMouse.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "MicroMouse.list"
	@echo 'Finished building: $@'
	@echo ' '

MicroMouse.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "MicroMouse.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

post-build:
	arm-none-eabi-objcopy -O ihex "MicroMouse.elf" "MicroMouse.hex"
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
