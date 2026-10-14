#!/usr/bin/env bash
# 诊断：定位 libnds 启动符号(initSystem/__libnds_exit等)所在的库，并列出模板 Makefile
echo '===== 全盘扫描启动符号所在库 ====='
found=0
for a in $(find /opt/devkitpro -name '*.a' 2>/dev/null); do
  hit=$(nm -C "$a" 2>/dev/null | grep -E 'initSystem|__libnds_exit|__libnds_mpu_setup|__dsimode|__secure_area__')
  if [ -n "$hit" ]; then echo "-- $a"; echo "$hit"; found=1; fi
done
[ $found -eq 0 ] && echo "（在所有 .a 中均未找到这些符号）"

echo '===== crt0 对象位置 ====='
find /opt/devkitpro -name 'ds_arm9_crt0.o' 2>/dev/null

echo '===== NDS 模板/示例 Makefile ====='
find /opt/devkitpro -type f -name 'Makefile' -path '*nds*' 2>/dev/null | head -10
find /opt/devkitpro/examples -maxdepth 4 -name 'Makefile' 2>/dev/null | head -10
