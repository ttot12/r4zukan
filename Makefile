#---------------------------------------------------------------------------------
# R4 黑白图鉴菜单 · 经典兼容版 NDS —— 无 libnds，纯寄存器驱动
# 产出老 R4 内核 / DeSmuME 都能加载的经典格式 (ARM9 入口 0x02000000)
#---------------------------------------------------------------------------------
.SUFFIXES:

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

export PATH := $(DEVKITARM)/bin:$(DEVKITPRO)/tools/bin:$(PATH)

TARGET   := r4zukan

# 经典 ARM9 编译参数
ARCH := -mcpu=arm946e-s -marm -O2 -ffunction-sections -fdata-sections

all: $(TARGET).nds

# 打包：-9 给 ARM9 ELF，-7 给 calico 的 ARM7 桩(仅作保持DS存活的桩)
$(TARGET).nds: $(TARGET).elf
	ndstool -c $(TARGET).nds -9 $(TARGET).elf -7 $(DEVKITPRO)/calico/bin/ds7_lykoi.elf

$(TARGET).elf: start.o main.o arm9.ld
	arm-none-eabi-gcc -nostdlib -nostartfiles -T arm9.ld -Wl,--gc-sections -o $@ start.o main.o

start.o: start.s
	arm-none-eabi-gcc $(ARCH) -c $< -o $@

main.o: main.c
	arm-none-eabi-gcc $(ARCH) -c $< -o $@

clean:
	rm -f *.o *.elf *.nds
