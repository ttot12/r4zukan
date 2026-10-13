#---------------------------------------------------------------------------------
# R4 黑白图鉴菜单 · 原创 NDS 启动器 —— devkitARM 构建文件
# 用法：装好 devkitPro(含 devkitARM) 后，在本目录运行 `make`，得到 r4zukan.nds
#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "请先设置 DEVKITARM 环境变量，例如：export DEVKITARM=/c/devkitPro/devkitARM")
endif

include $(DEVKITARM)/ds_rules

TARGET   := r4zukan
ROMTITLE := R4ZUKAN

ARCH := -marm -mthumb-interwork -march=armv5te -mtune=arm946e-s \
        -fomit-frame-pointer -ffast-math

CFLAGS   := -g -Wall -O2 $(ARCH) $(INCLUDE)
CXXFLAGS := $(CFLAGS) -fno-rtti -fno-exceptions -fno-threadsafe-statics
LDFLAGS  := -specs=ds_arm9.specs -g
LIBS     := -lnds9

all: $(TARGET).nds

$(TARGET).nds: $(TARGET).arm9
	ndstool -c $(TARGET).nds -9 $(TARGET).arm9

$(TARGET).arm9: main.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET).nds $(TARGET).arm9 main.o
