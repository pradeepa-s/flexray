################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./FlexRay.obj" \
"./Fr.obj" \
"./gio.obj" \
"./led_demo.obj" \
"./pinmux.obj" \
"./sys_core.obj" \
"./sys_esm.obj" \
"./sys_intvecs.obj" \
"./sys_main.obj" \
"./sys_memory.obj" \
"./sys_mpu.obj" \
"./sys_phantom.obj" \
"./sys_pmu.obj" \
"./sys_startup.obj" \
"./system.obj" \
"../sys_link.cmd" \
$(GEN_CMDS__FLAG) \
-l"rtsv7R4_T_be_v3D16_eabi.lib" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
FlexRay_CCSv5_example_code_nodeA.out \

EXE_OUTPUTS__QUOTED += \
"FlexRay_CCSv5_example_code_nodeA.out" \

BIN_OUTPUTS += \
FlexRay_CCSv5_example_code_nodeA.hex \

BIN_OUTPUTS__QUOTED += \
"FlexRay_CCSv5_example_code_nodeA.hex" \


# All Target
all: FlexRay_CCSv5_example_code_nodeA.out

# Tool invocations
FlexRay_CCSv5_example_code_nodeA.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Linker'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 --abi=eabi -g --display_error_number --diag_warning=225 --enum_type=packed -z -m"FlexRay_CCSv5_example_code_nodeA.map" --heap_size=0x800 --stack_size=0x800 -i"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/lib" -i"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/include" --reread_libs --warn_sections --xml_link_info="FlexRay_CCSv5_example_code_nodeA_linkInfo.xml" --rom_model --be32 -o "FlexRay_CCSv5_example_code_nodeA.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

FlexRay_CCSv5_example_code_nodeA.hex: $(EXE_OUTPUTS)
	@echo 'Invoking: ARM Hex Utility'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armhex"  -o "FlexRay_CCSv5_example_code_nodeA.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)$(BIN_OUTPUTS__QUOTED)
	-$(RM) "FlexRay.pp" "Fr.pp" "gio.pp" "led_demo.pp" "pinmux.pp" "sys_esm.pp" "sys_main.pp" "sys_phantom.pp" "sys_startup.pp" "system.pp" 
	-$(RM) "FlexRay.obj" "Fr.obj" "gio.obj" "led_demo.obj" "pinmux.obj" "sys_core.obj" "sys_esm.obj" "sys_intvecs.obj" "sys_main.obj" "sys_memory.obj" "sys_mpu.obj" "sys_phantom.obj" "sys_pmu.obj" "sys_startup.obj" "system.obj" 
	-$(RM) "sys_core.pp" "sys_intvecs.pp" "sys_memory.pp" "sys_mpu.pp" "sys_pmu.pp" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

