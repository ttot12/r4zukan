#!/usr/bin/env bash
# 诊断2：打印官方 NDS 模板 Makefile 与 specs/crt0 位置，判断 libnds2 正确链接方式
echo '===== 官方 NDS 模板 Makefile ====='
cat /opt/devkitpro/examples/nds/templates/Makefile 2>/dev/null
echo '===== 模板目录内容 ====='
ls -la /opt/devkitpro/examples/nds/templates/ 2>/dev/null
echo '===== 所有 .specs 文件 ====='
find /opt/devkitpro -name '*.specs' 2>/dev/null
echo '===== devkitARM 下所有 crt 相关 ====='
find /opt/devkitpro/devkitARM -name '*crt*' 2>/dev/null
echo '===== libnds 下 crt/specs 相关 ====='
find /opt/devkitpro/libnds \( -name '*crt*' -o -name '*.specs' \) 2>/dev/null
