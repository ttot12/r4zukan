#!/usr/bin/env bash
# 诊断3：找 default ARM7 桩 elf 的正确路径 + 打印官方 arm9 最小工程 Makefile
echo '===== 官方 arm9 模板 Makefile ====='
cat /opt/devkitpro/examples/nds/templates/arm9/Makefile 2>/dev/null
echo '===== arm9 模板目录 ====='
ls -laR /opt/devkitpro/examples/nds/templates/arm9/ 2>/dev/null | head -40
echo '===== 全盘找 default.elf / arm7 elf ====='
find /opt/devkitpro -name 'default.elf' 2>/dev/null
find /opt/devkitpro -iname '*arm7*.elf' 2>/dev/null | head
find /opt/devkitpro -name '*.elf' 2>/dev/null | head -20
echo '===== libnds 根目录 ====='
ls -la /opt/devkitpro/libnds/ 2>/dev/null