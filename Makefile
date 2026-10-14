#---------------------------------------------------------------------------------
# R4 黑白图鉴菜单 · 原创 NDS 启动器 —— devkitARM 构建文件
# 用法：装好 devkitPro(含 devkitARM) 后，在本目录运行 `make`，得到 r4zukan.nds
#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif
#---------------------------------------------------------------------------------
# 关键：把 devkitARM 和 devkitPro 的工具目录加进 PATH，否则找不到 ndstool / arm-none-eabi-g++
#---------------------------------------------------------------------------------
export PATH := $(DEVKITARM)/bin:$(DEVKITPRO)/tools/bin:$(PATH)

#---------------------------------------------------------------------------------
# devkitPro 的 ds_rules 提供 LIBNDS、INCLUDE 等标准变量
#---------------------------------------------------------------------------------
include $(DEVKITARM)/ds_rules

TARGET   := r4zukan
ROMTITLE := R4ZUKAN

ARCH := -marm -mthumb-interwork -march=armv5te -mtune=arm946e-s \
        -fomit-frame-pointer -ffast-math

# 关键：libnds 头文件路径 + 平台宏(NDS+ARM9，新版 libnds/calico 必需) + calico 头文件路径
INCLUDES := -I$(DEVKITPRO)/libnds/include -I$(DEVKITPRO)/libfat/include \
            -I$(DEVKITPRO)/maxmod/include -I$(DEVKITPRO)/calico/include
CFLAGS   := -g -Wall -O2 $(ARCH) -DNDS -DARM9 $(INCLUDES) $(INCLUDE)
CXXFLAGS := $(CFLAGS) -fno-rtti -fno-exceptions -fno-threadsafe-statics
LDFLAGS  := -specs=ds_arm9.specs -g $(ARCH)
# 关键：显式给出 libnds 的 -L 库路径，否则链接器找不到 libnds9.a
LIBS     := -L$(DEVKITPRO)/libnds/lib -lnds9 -lm

all: $(TARGET).nds

$(TARGET).nds: $(TARGET).arm9
	ndstool -c $(TARGET).nds -9 $(TARGET).arm9

$(TARGET).arm9: main.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET).nds $(TARGET).arm9 main.o
