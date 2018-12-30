# makefile v2.0
# Решение рекурсивного поиска исходных текстов с кодом, построение путей относительно корневой директории.
# (Корневая директория определяется местоположением makefile)
#=========================================================================================================
.PHONY: clean build flash

name_project = ws2812

OPTIMIZATION = 0

CPU_FREQ = 24000000

# Глобальные 
#DEFINE_CONST =  -DSTM32F103C8
DEFINE_CONST += -DSTM32F10X_MD
# DEFINE_CONST += -D__GNUC__
#DEFINE_CONST += -D__ASSEMBLY__

# Общие флаги компиляции
COMMON_FLAGS  = $(H_FOLDER_LIST)
COMMON_FLAGS += -O$(OPTIMIZATION)
COMMON_FLAGS += -gdwarf-2
COMMON_FLAGS += -Wall
COMMON_FLAGS += -c
COMMON_FLAGS += -fmessage-length=0
COMMON_FLAGS += -fno-builtin
COMMON_FLAGS += -ffunction-sections
COMMON_FLAGS += -fdata-sections
COMMON_FLAGS += -msoft-float
COMMON_FLAGS += -mapcs-frame
COMMON_FLAGS += -D__thumb2__=1
COMMON_FLAGS += -mno-sched-prolog
COMMON_FLAGS += -mtune=cortex-m3
COMMON_FLAGS += -mcpu=cortex-m3
COMMON_FLAGS += -mthumb
COMMON_FLAGS += -mfix-cortex-m3-ldrd
COMMON_FLAGS += -ffast-math
COMMON_FLAGS += -DCLK_CPU=$(CPU_FREQ)
COMMON_FLAGS += $(DEFINE_CONST)
# COMMON_FLAGS += -M


# Флаги компиляции специфичные для Си
CFLAG  = $(COMMON_FLAGS)
CFLAG += -std=c99
# CFLAG += -fno-strict-aliasing
# CFLAG += -fno-hosted

# Флаги компиляции специфичные для Си++
CppFLAGS = $(COMMON_FLAGS)


#Флаги компановщика
FLAGS_LD = -Xlinker -Map=$(MAIN_OUT_PATH)$(name_project).map
FLAGS_LD += -Wl,--gc-sections
FLAGS_LD += -mcpu=cortex-m3
FLAGS_LD += -mthumb
FLAGS_LD += -static 
FLAGS_LD += -T$(LinkerPATH)

#Флаги Ассемблера
FLAGS_ASM  = -D__ASSEMBLY__
FLAGS_ASM += $(CFLAG)
FLAGS_ASM += -g
FLAGS_ASM += -I. -x assembler-with-cpp
#================================================

#Префикс
CompilerPrefix = arm-none-eabi-

#Путь к скрипту компановщика
LinkerPATH = cmsis/stm32_flash_malloc.ld
# LinkerPATH = cmsis/stm32_flash_backaup.ld
# LinkerPATH = cmsis/stm32_flash.ld
# LinkerPATH = mcu/stm32f4xx_flash.ld

#Путь сохранения объектных файлов
OBJPATH = ./out/obj_files/

#Путь сохранения файлов .hex и .bin
MAIN_OUT_PATH = ./out/hexbin/








# Путь к консольному программатору st-link utility
# FLASHER = C:\Program Files (x86)\STMicroelectronics\STM32 ST-LINK Utility\ST-LINK Utility\ST-LINK_CLI.exe 
# Параметры программатора
# Для Windows
# FLASHER_PRAM  = -c SWD UR
# FLASHER_PRAM += -P "$(MAIN_OUT_PATH)$(name_project).bin" 0x08000000
# FLASHER_PRAM += -Rst
# Для Линукс
# [--debug] [--reset] [--format <format>] {read|write} /dev/sgX <path> <addr> <size>
FLASHER = st-flash
FLASHER_PRAM  = write $(MAIN_OUT_PATH)/$(name_project).bin 0x08000000

# Настройка отладчика openocd
# DB_SERVER =  D:\GNU_Tools_ARM_Embedded\openocd\bin/openocd.exe
# OPENOCD_PARAM = -c "source [find interface/stlink-v2.cfg]"
# OPENOCD_PARAM += -c "transport select hla_swd"
# OPENOCD_PARAM += -c "source [find target/stm32f1x.cfg]"

# OPENOCD_PARAM_DEBUG = $(OPENOCD_PARAM)
# OPENOCD_PARAM_DEBUG += -c "gdb_port 3333"
# OPENOCD_PARAM_DEBUG += -c "debug_level 2"
# OPENOCD_PARAM_DEBUG += -c "set WORKAREASIZE 0x2000"
# OPENOCD_PARAM_DEBUG += -c "reset_config srst_only"

#===========================================================================================
# Рекурсивный вариант функции wildcard
rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

SRC_TEMPLATE = *.c *.cpp *.s *.S
LIB_TEMPLATE = *.a *.so
H_TEMPLATE   = *.h

# Построение списков каталогов
# FOLDER_LIST = $(sort $(dir $(wildcard **/)) $(dir $(call rwildcard,,*)))
H_FOLDER_LIST = $(addprefix -I, $(sort $(dir $(call rwildcard,,$(H_TEMPLATE)))))
SRC_FOLDER_LIST = $(sort $(dir $(dir $(call rwildcard,,$(SRC_TEMPLATE)))))
LIB_FOLDER_LIST = $(sort $(dir $(dir $(call rwildcard,,$(LIB_TEMPLATE)))))


# Построение списков исходных файлов
C_FILE   = $(notdir $(call rwildcard,,*.c))
H_FILE   = $(notdir $(call rwildcard,,*.h))
Cpp_FILE = $(notdir $(call rwildcard,,*.cpp))
ASM_FILE = $(notdir $(call rwildcard,,*.s *.S))

# Получения списка обектных файлов
OBJ_FILES  = $(patsubst %.c,%.o,$(C_FILE))
OBJ_FILES += $(patsubst %.cpp,%.o,$(Cpp_FILE))
OBJ_FILES += $(patsubst %.s,%.o,$(ASM_FILE))

OBJ = $(addprefix $(OBJPATH), $(OBJ_FILES))

# TODO Возможно стоит пересмотреть эту строку
VPATH = $(SRC_FOLDER_LIST) $(OBJPATH)
#============================================================================================




build: rm_elf $(name_project).elf

$(name_project).elf: $(OBJ_FILES)
	@echo '----------------------------------------------------------------'
	@echo Linking ...
	@$(CompilerPrefix)gcc $(FLAGS_LD)  $(OBJ) -o $(MAIN_OUT_PATH)$(name_project).elf

	@echo '----------------------------------------------------------------'
	
	@$(CompilerPrefix)objcopy -O binary $(MAIN_OUT_PATH)$(name_project).elf $(MAIN_OUT_PATH)$(name_project).bin
	@$(CompilerPrefix)objcopy -O ihex $(MAIN_OUT_PATH)$(name_project).elf $(MAIN_OUT_PATH)$(name_project).hex
	@$(CompilerPrefix)size $(MAIN_OUT_PATH)$(name_project).elf $(MAIN_OUT_PATH)$(name_project).hex
	@echo Build Comlite.


%.o: %.c makefile #$(H_FILE) 
	@echo Compiling $<
	@$(CompilerPrefix)gcc $(CFLAG) $< -o $(OBJPATH)$@


%.o: %.cpp makefile #$(H_FILE)
	@echo Compiling $<
	@$(CompilerPrefix)gcc $(CppFLAGS) $< -o $(OBJPATH)$@


%.o: %.s makefile #$(H_FILE)
	@echo Compiling $<
	@$(CompilerPrefix)gcc $(FLAGS_ASM) $< -o $(OBJPATH)$@
	

clean:
	@echo Delete object files...
	@rm -rf $(OBJPATH)*
	@rm -rf $(MAIN_OUT_PATH)*

rm_elf:
	@rm -rf $(MAIN_OUT_PATH)*


flash:
	#$(FLASHER) --reset erase 
	$(FLASHER) --reset $(FLASHER_PRAM)

debug:
	# st-util &
	$(CompilerPrefix)gdb $(MAIN_OUT_PATH)$(name_project).elf
# $(DB_SERVER) $(OPENOCD_PARAM_DEBUG)

test: 
	@echo $(H_FILE)
